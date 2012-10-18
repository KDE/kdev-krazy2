# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Since Qt 4.6.0, this definition is needed for GUI testing.
# It is backwards compatible with previous Qt versions, unlike the alternative
# which is to add #include <QTestGui> in the test files.
add_definitions(-DQT_GUI_LIB)

macro(UNIT_TESTS)
    foreach(_className ${ARGN})
        set(_testName ${_className}test)
        kde4_add_unit_test(${_testName} TESTNAME unit-${_testName} ${_testName}.cpp)
        target_link_libraries(${_testName}
            kdevkrazy2_library
            ${KDEVPLATFORM_TESTS_LIBRARIES}
            ${QT_QTTEST_LIBRARY}
        )
    endforeach(_className)
endmacro(UNIT_TESTS)

unit_tests(
    analysisjob
    analysisparameters
    analysisresults
    checker
    checkerlistjob
    checkerlistparser
    checkermodel
    issue
    issuemodel
    issuewidget
    krazy2view
    progressparser
    resultparser
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
    analysisresults
    checker
    checkerlistjob
    checkerlistparser
    checkermodel
    issue
    issuemodel
    #issuewidget # Disabled by default, as it uses KDevelop::TestCore and thus needs a lot of memory to perform the test
    #krazy2view # Disabled by default, as it uses KDevelop::TestCore and thus needs a lot of memory to perform the test
    progressparser
    resultparser
    selectcheckerswidget
    selectpathswidget
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
    examples/subdirectory/singleIssue.desktop
    examples/subdirectory/severalIssuesSeveralCheckers.qml
)

foreach(example ${examples})
    file(READ ${example} exampleContents)
    string(REPLACE "//krazy:skip\n" "" exampleContents "${exampleContents}")
    string(REPLACE "#krazy:skip\n" "" exampleContents "${exampleContents}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${example} "${exampleContents}")
endforeach(example)
