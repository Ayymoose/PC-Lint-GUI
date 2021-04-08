#include "PCLintPlusTest.h"
#include "../PC-Lint GUI/PCLintPlus.h"
#include <cstdlib>

namespace Test
{


void PCLintPlusTest::pclintplusParseSourceFileTest() noexcept
{
    Lint::PCLintPlus lint;

    QFile file(R"(..\PC-Lint GUI Test\data\pc-lint-plus\parseSourceFile.xml)");
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    QByteArray data = file.readAll();
    Q_ASSERT(data.size());
    file.close();

    try
    {
        auto sourceFiles = lint.processSourceFiles(data);
        TEST_COMPARE(sourceFiles.size(), (size_t)29);
    }
    catch (const std::exception& e)
    {
        qCritical() << e.what();
    }
}

void PCLintPlusTest::pclintplusGroupLintMessagesTest() noexcept
{
    Lint::PCLintPlus lint1;

    QFile file1(R"(..\PC-Lint GUI Test\data\pc-lint-plus\groupLintMessages1.xml)");
    file1.open(QIODevice::ReadOnly);
    Q_ASSERT(file1.isOpen());
    QByteArray data1 = file1.readAll();
    Q_ASSERT(data1.size());
    file1.close();

    try
    {
        auto lintMessages1 = lint1.parseLintMessages(data1);
        TEST_COMPARE(lintMessages1.size(), (size_t)(22));

        int errors = 0;
        int warnings = 0;
        int informations = 0;
        int supplementals = 0;
        for (auto const& message : lintMessages1)
        {
            if (message.type == Lint::Type::TYPE_WARNING)
            {
                warnings++;
            }
            else if (message.type == Lint::Type::TYPE_INFORMATION)
            {
                informations++;
            }
            else if (message.type == Lint::Type::TYPE_ERROR)
            {
                errors++;
            }
            else if (message.type == Lint::Type::TYPE_SUPPLEMENTAL)
            {
                supplementals++;
            }
            else
            {
                Q_ASSERT(false);
            }
        }

        TEST_COMPARE(warnings, 7);
        TEST_COMPARE(errors, 2);
        TEST_COMPARE(informations, 4);
        TEST_COMPARE(supplementals, 9);

        // Group messages together
        auto groupedLintMessages = lint1.groupLintMessages(std::move(lintMessages1));

        TEST_COMPARE(groupedLintMessages.size(), size_t(13));

        auto group1 = groupedLintMessages[0];
        TEST_COMPARE(group1.size(), size_t(1));
        TEST_COMPARE(group1[0].type, Lint::Type::TYPE_WARNING);

        auto group2 = groupedLintMessages[1];
        TEST_COMPARE(group2.size(), size_t(2));
        TEST_COMPARE(group2[0].type, Lint::Type::TYPE_WARNING);
        TEST_COMPARE(group2[1].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group3 = groupedLintMessages[2];
        TEST_COMPARE(group3.size(), size_t(1));
        TEST_COMPARE(group3[0].type, Lint::Type::TYPE_WARNING);

        auto group4 = groupedLintMessages[3];
        TEST_COMPARE(group4.size(), size_t(4));
        TEST_COMPARE(group4[0].type, Lint::Type::TYPE_INFORMATION);
        TEST_COMPARE(group4[1].type, Lint::Type::TYPE_SUPPLEMENTAL);
        TEST_COMPARE(group4[2].type, Lint::Type::TYPE_SUPPLEMENTAL);
        TEST_COMPARE(group4[3].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group5 = groupedLintMessages[4];
        TEST_COMPARE(group5.size(), size_t(2));
        TEST_COMPARE(group5[0].type, Lint::Type::TYPE_ERROR);
        TEST_COMPARE(group5[1].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group6 = groupedLintMessages[5];
        TEST_COMPARE(group6.size(), size_t(1));
        TEST_COMPARE(group6[0].type, Lint::Type::TYPE_WARNING);

        auto group7 = groupedLintMessages[6];
        TEST_COMPARE(group7.size(), size_t(3));
        TEST_COMPARE(group7[0].type, Lint::Type::TYPE_ERROR);
        TEST_COMPARE(group7[1].type, Lint::Type::TYPE_SUPPLEMENTAL);
        TEST_COMPARE(group7[2].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group8 = groupedLintMessages[7];
        TEST_COMPARE(group8.size(), size_t(1));
        TEST_COMPARE(group8[0].type, Lint::Type::TYPE_WARNING);

        auto group9 = groupedLintMessages[8];
        TEST_COMPARE(group9.size(), size_t(2));
        TEST_COMPARE(group9[0].type, Lint::Type::TYPE_WARNING);
        TEST_COMPARE(group9[1].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group10 = groupedLintMessages[9];
        TEST_COMPARE(group10.size(), size_t(1));
        TEST_COMPARE(group10[0].type, Lint::Type::TYPE_WARNING);

        auto group11 = groupedLintMessages[10];
        TEST_COMPARE(group11.size(), size_t(2));
        TEST_COMPARE(group11[0].type, Lint::Type::TYPE_INFORMATION);
        TEST_COMPARE(group11[1].type, Lint::Type::TYPE_SUPPLEMENTAL);

        auto group12 = groupedLintMessages[11];
        TEST_COMPARE(group12.size(), size_t(1));
        TEST_COMPARE(group12[0].type, Lint::Type::TYPE_INFORMATION);

        auto group13 = groupedLintMessages[12];
        TEST_COMPARE(group13.size(), size_t(1));
        TEST_COMPARE(group13[0].type, Lint::Type::TYPE_INFORMATION);

    }
    catch (const std::exception& e)
    {
        qCritical() << e.what();
    }

    Lint::PCLintPlus lint2;

    QFile file2(R"(..\PC-Lint GUI Test\data\pc-lint-plus\groupLintMessages2.xml)");
    file2.open(QIODevice::ReadOnly);
    Q_ASSERT(file2.isOpen());
    QByteArray data2 = file2.readAll();
    Q_ASSERT(data2.size());
    file2.close();

    try
    {
        auto lintMessages2 = lint2.parseLintMessages(data2);
        TEST_COMPARE(lintMessages2.size(), (size_t)(8));

        int errors = 0;
        int warnings = 0;
        int informations = 0;
        int supplementals = 0;
        for (auto const& message : lintMessages2)
        {
            if (message.type == Lint::Type::TYPE_WARNING)
            {
                warnings++;
            }
            else if (message.type == Lint::Type::TYPE_INFORMATION)
            {
                informations++;
            }
            else if (message.type == Lint::Type::TYPE_ERROR)
            {
                errors++;
            }
            else if (message.type == Lint::Type::TYPE_SUPPLEMENTAL)
            {
                supplementals++;
            }
            else
            {
                Q_ASSERT(false);
            }
        }

        TEST_COMPARE(warnings, 2);
        TEST_COMPARE(errors, 2);
        TEST_COMPARE(informations, 4);
        TEST_COMPARE(supplementals, 0);

        // Group messages together
        auto groupedLintMessages = lint1.groupLintMessages(std::move(lintMessages2));

        TEST_COMPARE(groupedLintMessages.size(), size_t(8));

        auto group1 = groupedLintMessages[0];
        TEST_COMPARE(group1.size(), size_t(1));
        TEST_COMPARE(group1[0].type, Lint::Type::TYPE_WARNING);

        auto group2 = groupedLintMessages[1];
        TEST_COMPARE(group2.size(), size_t(1));
        TEST_COMPARE(group2[0].type, Lint::Type::TYPE_INFORMATION);

        auto group3 = groupedLintMessages[2];
        TEST_COMPARE(group3.size(), size_t(1));
        TEST_COMPARE(group3[0].type, Lint::Type::TYPE_INFORMATION);

        auto group4 = groupedLintMessages[3];
        TEST_COMPARE(group4.size(), size_t(1));
        TEST_COMPARE(group4[0].type, Lint::Type::TYPE_WARNING);

        auto group5 = groupedLintMessages[4];
        TEST_COMPARE(group5.size(), size_t(1));
        TEST_COMPARE(group5[0].type, Lint::Type::TYPE_INFORMATION);

        auto group6 = groupedLintMessages[5];
        TEST_COMPARE(group6.size(), size_t(1));
        TEST_COMPARE(group6[0].type, Lint::Type::TYPE_ERROR);

        auto group7 = groupedLintMessages[6];
        TEST_COMPARE(group7.size(), size_t(1));
        TEST_COMPARE(group7[0].type, Lint::Type::TYPE_ERROR);

        auto group8 = groupedLintMessages[7];
        TEST_COMPARE(group8.size(), size_t(1));
        TEST_COMPARE(group8[0].type, Lint::Type::TYPE_INFORMATION);

    }
    catch (const std::exception& e)
    {
        qCritical() << e.what();
    }
}

void PCLintPlusTest::pclintplusConsumeLintChunkTest() noexcept
{
    Lint::PCLintPlus lint1;

    QFile file1(R"(..\PC-Lint GUI Test\data\pc-lint-plus\chunk1.xml)");
    file1.open(QIODevice::ReadOnly);
    Q_ASSERT(file1.isOpen());
    QByteArray data1 = file1.readAll();
    Q_ASSERT(data1.size());
    file1.close();

    // Simulate incoming lint chunk data

    const int CHUNK_SIZE = 39 + std::rand() % 111;

    int firstIndex = 0;
    int secondIndex = CHUNK_SIZE;

    std::vector<QByteArray> moduleData1;

    while (firstIndex < data1.size())
    {
        // Grab a chunk of data
        auto moduleChunk = data1.mid(firstIndex, secondIndex);

        auto modules = lint1.stitchModule(moduleChunk);
        for (auto const& module : modules)
        {
            if (module.size() > 0)
            {
                moduleData1.emplace_back(module);
            }
        }

        firstIndex += CHUNK_SIZE;
    }

    TEST_COMPARE(moduleData1.size(), size_t(11));


    Lint::PCLintPlus lint2;

    QFile file2(R"(..\PC-Lint GUI Test\data\pc-lint-plus\chunk2.xml)");
    file2.open(QIODevice::ReadOnly);
    Q_ASSERT(file2.isOpen());
    QByteArray data2 = file2.readAll();
    Q_ASSERT(data2.size());
    file2.close();

    firstIndex = 0;
    secondIndex = CHUNK_SIZE;

    std::vector<QByteArray> moduleData2;

    while (firstIndex < data2.size())
    {
        // Grab a chunk of data
        auto moduleChunk = data2.mid(firstIndex, secondIndex);

        auto modules = lint2.stitchModule(moduleChunk);
        for (auto const& module : modules)
        {
            if (module.size() > 0)
            {
                moduleData2.emplace_back(module);
            }
        }

        firstIndex += CHUNK_SIZE;
    }

    TEST_COMPARE(moduleData2.size(), size_t(44));

}

}
