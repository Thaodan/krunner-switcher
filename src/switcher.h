/*
   Copyright 2018 by Tatu Lahtela <lahtela@iki.fi>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SWITCHER_H
#define SWITCHER_H

#include <KRunner/AbstractRunner>
#include <QtGui/qwindowdefs.h>

class KWindowInfo;


class Switcher : public KRunner::AbstractRunner {
Q_OBJECT

public:
    Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);

    ~Switcher() override;

    void match(KRunner::RunnerContext &context) override;

    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;

private Q_SLOTS:

    void prepareForMatchSession();

    void matchSessionComplete();

    void gatherInfo();

private:
    KRunner::QueryMatch windowMatch(const KWindowInfo &info, qreal relevance = 1.0,
                                    KRunner::QueryMatch::CategoryRelevance categoryRelevance = KRunner::QueryMatch::CategoryRelevance::Highest);

    QHash<WId, KWindowInfo> m_windows;
    QHash<WId, QIcon> m_icons;
    QStringList m_desktopNames;
};

#endif
