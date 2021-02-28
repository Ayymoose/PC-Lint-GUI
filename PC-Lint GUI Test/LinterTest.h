#pragma once

#include "Tester.h"
#include <map>
#include <QString>

namespace Test
{

class LinterTest : public TestFunction
{
public:
    constexpr LinterTest() = default;

    using LinterFunctionMap = const std::map<QString, bool (LinterTest::*)(void)>;

    LinterFunctionMap m_tests =
    {
        // PC-Lint Plus tests
        {"pclintplusLicenseErrorTest", &LinterTest::pclintplusLicenseErrorTest},
        {"pclintplusLintStuckTest", &LinterTest::pclintplusLintStuckTest},
        {"pclintplusLintAbortTest", &LinterTest::pclintplusLintAbortTest},
        {"lintUnsupportedVersionTest", &LinterTest::lintUnsupportedVersionTest},
        {"pclintplusProcessErrorTest", &LinterTest::pclintplusProcessErrorTest},
        {"pclintplusCPPProject0Test", &LinterTest::pclintplusCPPProject0Test},

        // PC-Lint tests
        {"pclintLicenseErrorTest", &LinterTest::pclintLicenseErrorTest},
        {"pclintLintStuckTest", &LinterTest::pclintLintStuckTest},
        {"pclintLintAbortTest", &LinterTest::pclintLintAbortTest},
        {"pclintProcessErrorTest", &LinterTest::pclintProcessErrorTest},

    };

private:
    bool pclintplusCPPProject0Test();
    bool pclintplusLicenseErrorTest();
    bool pclintplusLintStuckTest();
    bool pclintplusLintAbortTest();
    bool pclintplusProcessErrorTest();
    bool lintUnsupportedVersionTest();

    bool pclintLicenseErrorTest();
    bool pclintLintStuckTest();
    bool pclintLintAbortTest();
    bool pclintProcessErrorTest();
};

};
