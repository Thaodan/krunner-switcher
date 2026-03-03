cmake_minimum_required(VERSION 3.15)

project(switcher)

set(QT_MIN_VERSION "5.9.0")
set(KF6_MIN_VERSION "5.42.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(Qt6 ${QT_MIN_VERSION}
  CONFIG REQUIRED
  COMPONENTS
  Widgets)
# X11Extras
find_package(KF6 ${KF6_MIN_VERSION}
  REQUIRED COMPONENTS
  Runner
  I18n
  WindowSystem
  # Pulled in by Plasma
  Config)


include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

add_subdirectory(src)



feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
