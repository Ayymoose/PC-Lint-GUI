#pragma once
#include "Tester.h"
#include <map>
#include <QString>

namespace Test
{

const QString solutionPath = R"(..\PC-Lint GUI Test\data\solutions\)";
const QString microchipStudioSolutionPath = solutionPath + R"(microchip studio\)";
const QString visualStudioSolutionPath = solutionPath + R"(visual studio\)";

class ProjectSolutionTest
{
public:
    ProjectSolutionTest() = default;


    void runTests();

    using ProjectSolutionFunction = bool (ProjectSolutionTest::*)(void);

private:
    const std::map<QString, ProjectSolutionFunction> tests =
    {
        {"atmelStudioGarbageSolutionNameTest", &ProjectSolutionTest::atmelStudioGarbageSolutionNameTest},
        {"atmelStudioEmptySolutionTest", &ProjectSolutionTest::atmelStudioEmptySolutionTest},
        {"atmelStudioCSolutionTest", &ProjectSolutionTest::atmelStudioCSolutionTest},
        {"atmelStudioCPPSolutionTest", &ProjectSolutionTest::atmelStudioCPPSolutionTest},

        {"visualStudioCPPProjectTest", &ProjectSolutionTest::visualStudioCPPProjectTest},
        {"visualStudioCPPMultiProjectSolutionTest", &ProjectSolutionTest::visualStudioCPPMultiProjectSolutionTest}
    };

    bool atmelStudioGarbageSolutionNameTest();
    bool atmelStudioEmptySolutionTest();
    bool atmelStudioCSolutionTest();
    bool atmelStudioCPPSolutionTest();
    bool visualStudioCPPProjectTest();
    bool visualStudioCPPMultiProjectSolutionTest();


    int m_failedTests;
    int m_passedTests;
};

};
