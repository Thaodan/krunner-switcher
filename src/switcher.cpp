/***************************************************************************
 *   Copyright 2009 by Martin Gräßlin <kde@martin-graesslin.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/
#include "switcher.h"

#include "config-switcher.h"

#include <QTimer>
#include <QDebug>
#include <QIcon>
#include <KWindowSystem>
#include <KWindowInfo>
#include <KX11Extras>
#include <KLocalizedString>

#ifdef HAVE_X11

// #include <QtX11Extras/QX11Info>
#include <netwm.h>

#endif

/*

  FIXME: kf.coreaddons: The plugin "runner_switcher.so" explicitly states an Id in the embedded metadata,
  which is different from the one derived from the filename The Id field from the KPlugin object in the metadata should be removed

 */

K_PLUGIN_CLASS_WITH_JSON(Switcher , "plasma-runner-switcher.json")

Switcher::Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args)
        : AbstractRunner(parent, metaData) {
    Q_UNUSED(args);
    setObjectName(QLatin1String("Switcher"));


    addSyntax(KRunner::RunnerSyntax(i18nc("Note this is a KRunner keyword", ".<app name>"),
                                          i18n("Switch to application by typing a dot and the app name, e.g. '.emacs'")));

    connect(this, &KRunner::AbstractRunner::prepare, this, &Switcher::prepareForMatchSession);
    connect(this, &KRunner::AbstractRunner::teardown, this, &Switcher::matchSessionComplete);
}

Switcher::~Switcher() = default;

// Called in the main thread
void Switcher::gatherInfo() {
    for (const WId &w: KX11Extras::windows()) {
        KWindowInfo info(w, NET::WMWindowType | NET::WMDesktop |
                            NET::WMState | NET::XAWMState |
                            NET::WMName,
                         NET::WM2WindowClass | NET::WM2WindowRole | NET::WM2AllowedActions);
        if (info.valid() && info.name() != "KRunner — krunner") {
            // ignore NET::Tool and other special window types
            NET::WindowType wType = info.windowType(NET::NormalMask | NET::DesktopMask | NET::DockMask |
                                                    NET::ToolbarMask | NET::MenuMask | NET::DialogMask |
                                                    NET::OverrideMask | NET::TopMenuMask |
                                                    NET::UtilityMask | NET::SplashMask);

            if (wType != NET::Normal && wType != NET::Override && wType != NET::Unknown &&
                wType != NET::Dialog && wType != NET::Utility) {
                continue;
            }
            m_windows.insert(w, info);
            m_icons.insert(w, QIcon(KX11Extras::icon(w)));
        }
    }

    const int desktopCount = KX11Extras::numberOfDesktops();
    for (int i = 1; i <= desktopCount; ++i) {
        m_desktopNames.append(KX11Extras::desktopName(i));
    }
}

// Called in the main thread
void Switcher::prepareForMatchSession() {
    gatherInfo();
}

// Called in the main thread
void Switcher::matchSessionComplete() {
    m_desktopNames.clear();
    m_icons.clear();
    m_windows.clear();
}

// Called in the secondary thread
void Switcher::match(KRunner::RunnerContext &context) {
    const QString term = context.query();
    if (!context.isValid() || !term.startsWith(i18nc("Note this is a KRunner keyword", "."), Qt::CaseInsensitive)) return;

    QList<KRunner::QueryMatch> matches;

    // keyword match: when term starts with "window" we list all windows
    // the list can be restricted to windows matching a given name, class, role or desktop
    const QString windowName = term.mid(1, term.size());

    QHashIterator<WId, KWindowInfo> it(m_windows);
    while (it.hasNext()) {
        it.next();
        const WId w = it.key();
        const KWindowInfo info = it.value();
        const QString windowClass = QString::fromUtf8(info.windowClassName());

        // exclude not matching windows
        if (!KX11Extras::hasWId(w)) continue;

        if (!windowName.isEmpty() && !info.name().contains(windowName, Qt::CaseInsensitive) &&
            !windowClass.contains(windowName, Qt::CaseInsensitive)) {
            continue;
        }
        matches.append(windowMatch(info));
    }

    context.addMatches(matches);
}

// Called in the main thread
void Switcher::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) {
    Q_UNUSED(context)
    // check if it's a desktop
    if (match.id().startsWith(QLatin1String("windows_desktop"))) {
        KX11Extras::setCurrentDesktop(match.data().toInt());
        return;
    }

    WId w(match.data().toString().toULong());
    KX11Extras::forceActiveWindow(w);
}


KRunner::QueryMatch Switcher::windowMatch(const KWindowInfo &info, qreal relevance, const KRunner::QueryMatch::CategoryRelevance categoryRelevance) {
    KRunner::QueryMatch match(this);
    match.setCategoryRelevance(categoryRelevance);
    match.setData(QString::number(info.win()));
    match.setIcon(m_icons[info.win()]);
    match.setText(info.name());
    QString desktopName;
    int desktop = info.desktop();
    if (desktop == NET::OnAllDesktops) {
        desktop = KX11Extras::currentDesktop();
    }
    if (desktop <= m_desktopNames.size()) {
        desktopName = m_desktopNames[desktop - 1];
    } else {
        desktopName = KX11Extras::desktopName(desktop);
    }

    match.setSubtext(i18n("Activate running window on %1", desktopName));
    match.setRelevance(relevance);
    return match;
}


#include "switcher.moc"
