#include "ProjectSolutionTest.h"
#include "../PC-Lint GUI/ProjectSolution.h"

#include <QDebug>
#include <QFileInfo>


namespace Test
{

void ProjectSolutionTest::runTests()
{
    for (auto const& [testName, testFunction] : tests)
    {
        QString testResult;
        if ((this->*testFunction)())
        {
            testResult = "[OK]";
            m_passedTests++;
        }
        else
        {
            testResult = "[FAIL]";
            m_failedTests++;
        }
        qDebug().noquote() << "Running test " << testName << "\t" << testResult;
    }
    qDebug().nospace() << "# Passed tests : " << m_passedTests;
    qDebug().nospace() << "# Failed tests : " << m_failedTests;
    qDebug().nospace() << "Total tests run: " << (m_passedTests+m_failedTests);

}

bool ProjectSolutionTest::atmelStudioGarbageSolutionNameTest()
{
    // Try to build source files from a non-existent file name
    Lint::AtmelStudio7ProjectSolution solution;
    try
    {
        solution.buildSourceFiles("garbage");
        return false;
    }
    catch (const std::runtime_error& e)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectSolutionTest::atmelStudioEmptySolutionTest()
{
    // Try to build source files from an empty .cproj file
    Lint::AtmelStudio7ProjectSolution solution;
    try
    {
        solution.buildSourceFiles(microchipStudioSolutionPath + R"(empty_project\empty.cproj)");
        return false;
    }
    catch (const std::logic_error& e)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectSolutionTest::atmelStudioCSolutionTest()
{
    // Try to build source files a test solution with two files
    Lint::AtmelStudio7ProjectSolution solution;
    try
    {
        auto const files = solution.buildSourceFiles(microchipStudioSolutionPath + R"(c_project\c.cproj)");
        // Check we have main.c and test.c
        for (auto const& file : files)
        {
            auto const fileName = QFileInfo(file).fileName();
            if ((fileName != "main.c") && (fileName != "test.c"))
            {
                return false;
            }
        }
        return Test::compare(2, files.size());
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool ProjectSolutionTest::atmelStudioCPPSolutionTest()
{
    // Try to build source files from an CPP project
    Lint::AtmelStudio7ProjectSolution solution;
    try
    {
        auto const files = solution.buildSourceFiles(microchipStudioSolutionPath + R"(cpp_project\cpp.cppproj)");
        return Test::compare(2, files.size());
    }
    catch (...)
    {
        return false;
    }
}

};
