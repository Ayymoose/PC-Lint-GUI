#pragma once

#include "Tester.h"
#include <map>
#include <QString>

namespace Test
{

const QString solutionPath = R"(..\PC-Lint GUI Test\data\solutions\)";
const QString microchipStudioSolutionPath = solutionPath + R"(microchip studio\)";
const QString visualStudioSolutionPath = solutionPath + R"(visual studio\)";
const QString qtStudioSolutionPath = solutionPath + R"(qt\)";

class ProjectSolutionTest : public TestFunction
{
public:
    constexpr ProjectSolutionTest() = default;

    using ProjectSolutionFunctionMap = const std::map<QString, bool (ProjectSolutionTest::*)(void)>;

    ProjectSolutionFunctionMap m_tests =
    {
        {"atmelStudioGarbageSolutionNameTest", &ProjectSolutionTest::atmelStudioGarbageSolutionNameTest},
        {"atmelStudioEmptySolutionTest", &ProjectSolutionTest::atmelStudioEmptySolutionTest},
        {"atmelStudioCSolutionTest", &ProjectSolutionTest::atmelStudioCSolutionTest},
        {"atmelStudioCPPSolutionTest", &ProjectSolutionTest::atmelStudioCPPSolutionTest},

        {"visualStudioGarbageSolutionNameTest", &ProjectSolutionTest::visualStudioGarbageSolutionNameTest},
        {"visualStudioEmptySolutionTest", &ProjectSolutionTest::visualStudioEmptySolutionTest},
        {"visualStudioCPPProjectTest", &ProjectSolutionTest::visualStudioCPPProjectTest},
        {"visualStudioCPPMultiProjectSolutionTest", &ProjectSolutionTest::visualStudioCPPMultiProjectSolutionTest},
        {"qtSolution", &ProjectSolutionTest::qtSolution}
    };

private:
    bool atmelStudioGarbageSolutionNameTest();
    bool atmelStudioEmptySolutionTest();
    bool atmelStudioCSolutionTest();
    bool atmelStudioCPPSolutionTest();

    bool visualStudioGarbageSolutionNameTest();
    bool visualStudioEmptySolutionTest();
    bool visualStudioCPPProjectTest();
    bool visualStudioCPPMultiProjectSolutionTest();
    bool qtSolution();
};

};
