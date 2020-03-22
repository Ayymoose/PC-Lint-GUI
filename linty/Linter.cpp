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


    // Test

    QString test = "D:\\Users\\Ayman\\Downloads\\thief_2_service_release\\thief_2_service_release\\rdrive\\prj\\thief2\\src\\DARK\\CRCUTTY.C";


    QProcess lintProcess;
    QStringList lintArguments;

    // This is the extra file we must pass into the linter to produce XML output
    QStringList xmlArguments;
    xmlArguments << "-v"
                 << "-width(0)"
                 << "+xml(?xml version=""1.0"" ?)"
                 << "-""format=<message><file>%f</file> <line>%l</line> <type>%t</type> <code>%n</code> <desc>%m</desc></message>"
                 << "-""format_specific= "
                 << "-hFs1"
                 << "-pragma(message)";

    // -v -width(0) +xml("?xml version="1.0" ?") +xml(doc) -"format=<message><file>%f</file> <line>%l</line> <type>%t</type> <code>%n</code> <desc>%m</desc></message>"

    lintProcess.setProcessChannelMode(QProcess::MergedChannels);

    // Add any options we have
    if (linterLintOptions != "")
    {
        lintArguments << linterLintOptions;
    }
    lintArguments << xmlArguments << linterFilePath << test;

    qDebug() << "Process: " << linterExecutable;
    qDebug() << "Arguments: " << lintArguments;

    lintProcess.setProgram(linterExecutable);
    lintProcess.setArguments(lintArguments);
    lintProcess.start();

    if (!lintProcess.waitForStarted())
    {
        return LINTER_TIMEOUT;
    }

    QByteArray lintData;

    while(lintProcess.waitForReadyRead())
    {
        lintData.append(lintProcess.readAll());
    }

    qDebug() << lintData.data();

    // Check linter version (if we can't determine the version then return error)
  /*  if (lintVersion != LINT_VERSION)
    {
        return LINTER_EXECUTABLE_UNKNOWN;
    }*/

    // linterExecutablePath - Path to the linter executable (including)
    // linterFilePath       - Path to the actual lint file (including)
    // linterLintOptions    - Command line options to pass to the linter
    // linterDirectory      - The directory to lint (for now)

    return LINTER_OK;
}
