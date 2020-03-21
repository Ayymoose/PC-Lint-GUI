#include "Linter.h"
#include <QDebug>
#include <QProcess>

// Currently only
// PC-Lint v9 is supported

LINTER_STATUS Linter::lint(const QString& linterExecutable, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory)
{
    // Possible reasons why lint would fail is

    // One of the inputs passed is empty or garbage
    // A non-lint executable was passed

    // Check linter version (if we can't determine the version then return error)
    QProcess lintProcess;
    lintProcess.setProgram(linterExecutable);
    lintProcess.start();

    if (!lintProcess.waitForStarted(1000))
    {
        return LINTER_TIMEOUT;
    }

    if (!lintProcess.waitForReadyRead(1000))
    {
        return LINTER_TIMEOUT;
    }

    qDebug() << "lint readAll: " << lintProcess.readAll();
    qDebug() << "lint stdout: " << lintProcess.readAllStandardOutput();
    QString lintVersion = lintProcess.readAllStandardError();
    qDebug() << "lint stderr: " << lintVersion;
    qDebug() << "process error: " << lintProcess.error();
    qDebug() << "process status: " << lintProcess.state();


    if (lintVersion != LINT_VERSION)
    {
        return LINTER_EXECUTABLE_UNKNOWN;
    }

    // Try to generate an XML file

    // linterExecutablePath - Path to the linter executable (including)
    // linterFilePath       - Path to the actual lint file (including)
    // linterLintOptions    - Command line options to pass to the linter
    // linterDirectory      - The directory to lint (for now)

    if (!lintProcess.waitForFinished(1000))
    {
        return LINTER_TIMEOUT;
    }

    return LINTER_OK;
}
