#include "LinterTest.h"
#include "../PC-Lint GUI/Linter.h"

namespace Test
{

bool LinterTest::pclintplusCPPProject0Test()
{
    Lint::Linter linter;

    // Try to lint a CPP project

    linter.setLintFile(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Test\data\pc-lint-plus\1.3.5\cpp_project_1\HelloWorld\pr_HelloWorld.lnt)");
    linter.setLintExecutable(R"(D:\Users\Ayman\Desktop\IDA PRO\pclp-1.3.5-windows-eval\windows\pclp64_debug.exe)");
    QSet<QString> files;
    files.insert(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Test\data\pc-lint-plus\1.3.5\cpp_project_1\HelloWorld\HelloWorld.cpp)");
    linter.setSourceFiles(files);
    try
    {
        auto result = linter.lint();
        auto goodResult = Test::compare(Lint::Status::LINTER_COMPLETE, result);

        auto const messages = linter.getLinterMessages();


        return goodResult;
    } catch (...)
    {
        return false;
    }
}

bool LinterTest::pclintplusLicenseErrorTest()
{
    Lint::Linter linter;

    // Test to check if there were any license errors
    // TODO: License file has to be in same directory as lint exe
    linter.setLintFile(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Test\data\pc-lint-plus\1.3.5\license_error\empty.lnt)");
    linter.setLintExecutable(R"(D:\Users\Ayman\Desktop\IDA PRO\pclp-1.3.5-windows-eval\windows\pclp64_debug.exe)");

    try
    {
        auto result = linter.lint();
        return Test::compare(Lint::Status::LINTER_LICENSE_ERROR, result);
    } catch (...)
    {
        return false;
    }
}

bool LinterTest::pclintplusLintStuckTest()
{
    return false;
}

bool LinterTest::pclintplusLintAbortTest()
{
    return false;
}

bool LinterTest::pclintplusProcessErrorTest()
{
    return false;
}

bool LinterTest::lintUnsupportedVersionTest()
{
    // Test to check if the executable is actually a PC-Lint executable
    Lint::Linter linter;
    linter.setLintFile(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Test\data\pc-lint-plus\1.3.5\license_error\empty.lnt)");
    linter.setLintExecutable("where.exe");
    try
    {
        auto result = linter.lint();
        return Test::compare(Lint::Status::LINTER_UNSUPPORTED_VERSION, result);
    } catch (...)
    {
        return false;
    }
}

bool LinterTest::pclintLicenseErrorTest()
{
    return false;
}

bool LinterTest::pclintLintStuckTest()
{
    return false;
}

bool LinterTest::pclintLintAbortTest()
{
    return false;
}

bool LinterTest::pclintProcessErrorTest()
{
    return false;
}

}
