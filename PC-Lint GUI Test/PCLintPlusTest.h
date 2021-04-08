#pragma once

#include "Tester.h"
#include <map>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QTest>

namespace Test
{

class PCLintPlusTest : public TestFunction
{
public:
    constexpr PCLintPlusTest() = default;

    using PCLintPlusFunctionMap = const std::map<QString, void (PCLintPlusTest::*)(void)>;

    PCLintPlusFunctionMap m_tests =
    {
        // PC-Lint Plus tests
        {"parseSourceFileTest", &PCLintPlusTest::pclintplusParseSourceFileTest},
        {"groupLintMessagesTest", &PCLintPlusTest::pclintplusGroupLintMessagesTest},
        {"consumeLintChunkTest", &PCLintPlusTest::pclintplusConsumeLintChunkTest},
        {"parseLintFileTest", &PCLintPlusTest::parseLintFileTest}
    };

private:

    void pclintplusParseSourceFileTest() noexcept;
    void pclintplusGroupLintMessagesTest() noexcept;
    void pclintplusConsumeLintChunkTest() noexcept;
    void parseLintFileTest() noexcept;
};

};
