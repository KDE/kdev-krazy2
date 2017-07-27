cmake_minimum_required(VERSION 2.8.12)

project(kdevkrazy2)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)

set(QT_MIN_VERSION "5.4.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Test)
set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS ItemModels TextEditor ThreadWeaver)
find_package(KDevPlatform 1.90.90 REQUIRED)

add_subdirectory(tests)

set(kdevkrazy2_library_SRCS
    analysisjob.cpp
    analysisparameters.cpp
    analysisprogressparser.cpp
    analysisresults.cpp
    analysisresultsparser.cpp
    checker.cpp
    checkerlistjob.cpp
    checkerlistparser.cpp
    checkermodel.cpp
    common.cpp
    issue.cpp
    settings/selectcheckerswidget.cpp
    settings/selectpathswidget.cpp
    settings/krazy2preferences.cpp
    settings/krazy2projectsettings.cpp
)

# Instead of compiling the plugin directly from the sources, the sources are
# compiled to a static library that is linked (and, being static, also embedded)
# in the plugin.
# As everything but a tiny initialization code is in a static library, the build
# system for the tests can be easily set up.
ki18n_wrap_ui(kdevkrazy2_library_SRCS
                  settings/selectcheckerswidget.ui
                  settings/selectpathswidget.ui
                  settings/krazy2settings.ui)

kconfig_add_kcfg_files(kdevkrazy2_library_SRCS settings/krazy2config.kcfgc)

add_library(kdevkrazy2_library STATIC ${kdevkrazy2_library_SRCS})
target_link_libraries(kdevkrazy2_library
    KDev::Interfaces
    KDev::Util
    KDev::Project
    KDev::Shell
    KDev::Language
    KF5::TextEditor
)

kdevplatform_add_plugin(kdevkrazy2 JSON kdevkrazy2.json SOURCES krazy2plugin.cpp)
target_link_libraries(kdevkrazy2
    kdevkrazy2_library
)

install(FILES kdevkrazy2.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevkrazy2)
