#pragma once

#include "Tester.h"
#include <map>
#include <QString>
#include <QByteArray>
#include <QFile>

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
        {"pclintplusParseSourceFileTest", &PCLintPlusTest::pclintplusParseSourceFileTest},
        {"pclintplusGroupLintMessagesTest", &PCLintPlusTest::pclintplusGroupLintMessagesTest},
        {"pclintplusConsumeLintChunkTest", &PCLintPlusTest::pclintplusConsumeLintChunkTest}

    };

private:

    void pclintplusParseSourceFileTest() noexcept;
    void pclintplusGroupLintMessagesTest() noexcept;
    void pclintplusConsumeLintChunkTest() noexcept;

    QByteArray simulateModuleChunk(const QByteArray& data) noexcept;
};

};
