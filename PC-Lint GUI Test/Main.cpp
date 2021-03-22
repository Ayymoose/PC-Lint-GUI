#include <QCoreApplication>

#include "LinterTest.h"

#include <vector>
#include <memory>


void messageHandler(QtMsgType type, const QMessageLogContext&, const QString &msg)
{
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stdout, "%s\n", msg.toLocal8Bit().constData());
        fflush(stdout);
        break;
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        break;
    }
}

int main(int , char *[])
{
    qInstallMessageHandler(messageHandler);

    // Add new tests here
    Test::PCLintPlusTest linterTest;

    Test::TestFunction testMain;
    testMain.runTests(&linterTest, linterTest.m_tests);

    return 0;
}
