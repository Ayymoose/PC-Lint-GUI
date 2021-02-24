#pragma once
#include "Tester.h"
#include <map>
#include <QString>

namespace Test
{

const QString solutionPath = R"(..\PC-Lint GUI Test\data\solutions\)";
const QString microchipStudioSolutionPath = solutionPath + R"(microchip studio\)";

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
        {"atmelStudioCPPSolutionTest", &ProjectSolutionTest::atmelStudioCPPSolutionTest}
    };

    bool atmelStudioGarbageSolutionNameTest();
    bool atmelStudioEmptySolutionTest();
    bool atmelStudioCSolutionTest();
    bool atmelStudioCPPSolutionTest();


    int m_failedTests;
    int m_passedTests;
};

};
