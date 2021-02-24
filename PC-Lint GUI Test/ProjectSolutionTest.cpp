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

bool ProjectSolutionTest::visualStudioCPPProjectTest()
{
    // Test a visual studio project with 1 source file
    Lint::VisualStudioProject visualStudioProject;
    try
    {
        auto const files = visualStudioProject.buildSourceFiles(visualStudioSolutionPath + R"(cpp_project\Project1.vcxproj)");
        return Test::compare(1, files.size());
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectSolutionTest::visualStudioCPPMultiProjectSolutionTest()
{
    // Test a visual studio project with 1 source file in 3 different projects
    Lint::VisualStudioProjectSolution visualStudioProjectSolution;
    try
    {
        auto const fileName = visualStudioSolutionPath + R"(cpp_solution\ConsoleApplication1\ConsoleApplication1.sln)";
        visualStudioProjectSolution.setDirectory(QFileInfo(fileName).canonicalPath());
        auto const files = visualStudioProjectSolution.buildSourceFiles(fileName);
        return Test::compare(3, files.size());
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectSolutionTest::qtSolution()
{
    // Test a qt project with some C++ files
    Lint::QtSolution qtSolution;
    try
    {
        auto const fileName = qtStudioSolutionPath + R"(cpp_project\PC-Lint GUI.pro)";
        auto const files = qtSolution.buildSourceFiles(fileName);
        return Test::compare(12, files.size());
    }
    catch (...)
    {
        return false;
    }
}

};
