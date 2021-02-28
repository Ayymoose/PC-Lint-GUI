#include <QCoreApplication>

#include "LinterTest.h"
#include "ProjectSolutionTest.h"

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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler(messageHandler);

    // Add new tests here
    Test::ProjectSolutionTest projectSolutionTest;
    Test::LinterTest linterTest;

    Test::TestFunction testMain;
    testMain.runTests(&projectSolutionTest, projectSolutionTest.m_tests);
    testMain.runTests(&linterTest, linterTest.m_tests);

    return a.exec();
}
