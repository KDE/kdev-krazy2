project(kdevkrazy2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${kdevkrazy2_SOURCE_DIR}/cmake/)

find_package(KDE4 REQUIRED)
find_package(KDevPlatform REQUIRED)

include(KDE4Defaults)

include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${KDE4_INCLUDES}
)

add_subdirectory(settings)
add_subdirectory(tests)

set(kdevkrazy2_library_SRCS
    analysisjob.cpp
    analysisparameters.cpp
    analysisresults.cpp
    checker.cpp
    checkerlistjob.cpp
    checkerlistparser.cpp
    checkermodel.cpp
    issue.cpp
    issuemodel.cpp
    issuewidget.cpp
    krazy2view.cpp
    progressparser.cpp
    resultparser.cpp
    selectcheckerswidget.cpp
    selectpathswidget.cpp
    sortedissuesproxymodel.cpp
)

# Instead of compiling the plugin directly from the sources, the sources are
# compiled to a static library that is linked (and, being static, also embedded)
# in the plugin.
# As everything but a tiny initialization code is in a static library, the build
# system for the tests can be easily set up.
kde4_add_ui_files(kdevkrazy2_library_SRCS
                  krazy2view.ui
                  selectcheckerswidget.ui
                  selectpathswidget.ui)
kde4_add_library(kdevkrazy2_library ${kdevkrazy2_library_SRCS})
target_link_libraries(kdevkrazy2_library
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

# In x86_64, linking fails when a static library is added to a shared library.
# In order to work, they must be compiled using -fPIC
add_definitions("-fPIC")

kde4_add_plugin(kdevkrazy2 krazy2plugin.cpp)
target_link_libraries(kdevkrazy2
    kdevkrazy2_library
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

install(TARGETS kdevkrazy2 DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES kdevkrazy2.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kdevkrazy2.rc DESTINATION ${DATA_INSTALL_DIR}/kdevkrazy2)
