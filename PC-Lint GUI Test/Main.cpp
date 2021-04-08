#include <QCoreApplication>

#include "PCLintPlusTest.h"

int main(int , char *[])
{
    // Add new tests here
    Test::PCLintPlusTest linterTest;

    Test::TestFunction testMain;
    testMain.runTests(&linterTest, linterTest.m_tests);

    return 0;
}
