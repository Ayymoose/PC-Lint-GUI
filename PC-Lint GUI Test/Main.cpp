#include <QCoreApplication>
#include "ProjectSolutionTest.h"


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

    // Run our tests
    Test::ProjectSolutionTest projectSolutionTest = Test::ProjectSolutionTest();
    projectSolutionTest.runTests();

    return a.exec();
}
