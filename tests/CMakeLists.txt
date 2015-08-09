# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Since Qt 4.6.0, this definition is needed for GUI testing.
# It is backwards compatible with previous Qt versions, unlike the alternative
# which is to add #include <QTestGui> in the test files.
add_definitions(-DQT_GUI_LIB)

macro(UNIT_TESTS)
    foreach(_className ${ARGN})
        set(_testName ${_className}test)
        ecm_add_test(${_testName} LINK_LIBRARIES kdevkrazy2_library KDev::Tests Qt5::Test)
    endforeach(_className)
endmacro(UNIT_TESTS)

unit_tests(
    analysisjob
    analysisparameters
    analysisprogressparser
    analysisresults
    analysisresultsparser
    checker
    checkerlistjob
    checkerlistparser
    checkermodel
    issue
    selectcheckerswidget
    selectpathswidget
)

macro(MEM_TESTS)
    foreach(_testname ${ARGN})
        add_test(unit-mem-${_testname} ${CMAKE_CURRENT_SOURCE_DIR}/runMemcheck.py ${CMAKE_CURRENT_BINARY_DIR}/${_testname}test ${CMAKE_CURRENT_BINARY_DIR})
    endforeach(_testname)
endmacro(MEM_TESTS)

mem_tests(
    # analysisjob # Disabled by default, as it uses KDevelop::TestCore and thus needs a lot of memory to perform the test
    analysisparameters
    analysisprogressparser
    analysisresults
    analysisresultsparser
    checker
    checkerlistjob
    checkerlistparser
    checkermodel
    issue
    # selectcheckerswidget # Disabled because of too many false positives.
    # selectpathswidget    # Disabled because of too many false positives.
)

# Copy example files for real Krazy2 tests to binary directory. The source files
# contain a "//krazy:skip" line to prevent the errors from being shown when
# analyzing the project itself. That line is removed from the examples in the
# binary directory for the errors to be shown in the tests.
set(examples
    examples/singleIssue.cpp
    examples/singleExtraIssue.cpp
    examples/singleIssueNonAsciiFileNameḶḷambión.cpp
    examples/.singleIssueHiddenUnixFileName.cpp
    examples/severalIssuesSingleChecker.cpp
    examples/severalIssuesSeveralCheckers.cpp
    examples/severalIssuesSeveralCheckersUnknownFileType.dqq
    examples/subdirectory/singleIssue.desktop.example.do-not-translate
    examples/subdirectory/severalIssuesSeveralCheckers.qml
)

foreach(example ${examples})
    file(READ ${example} exampleContents)
    string(REPLACE "//krazy:skip\n" "" exampleContents "${exampleContents}")
    string(REPLACE "#krazy:skip\n" "" exampleContents "${exampleContents}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${example} "${exampleContents}")
endforeach(example)

# Translatable string extraction from desktop files is not handled by
# Messages.sh; it is done automatically instead. So, as far as I know, there is
# no way to prevent the extraction from a specific .desktop file. However,
# singleIssue.desktop is just an example file used for the tests, and it should
# not be translated, as it would be a waste of the translators' time. Thus, the
# file in the source directory has an extension that will not be used for the
# automatic string extraction, and it is renamed to the .desktop extension in
# the build directory, where the tests are run.
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/examples/subdirectory/singleIssue.desktop.example.do-not-translate
            ${CMAKE_CURRENT_BINARY_DIR}/examples/subdirectory/singleIssue.desktop)
