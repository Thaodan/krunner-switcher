add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.switcher\")

set(switcher_SRCS switcher.cpp)

add_library(krunner_switcher MODULE ${switcher_SRCS})
target_link_libraries(krunner_switcher Qt6::Widgets KF6::WindowSystem KF6::I18n KF6::Runner)



install(TARGETS krunner_switcher DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kf${KF_MAJOR_VERSION}/krunner)
