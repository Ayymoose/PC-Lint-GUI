#include "Linter.h"
#include <QDebug>


// Currently only
// PC-Lint v9 is supported

LINTER_STATUS Linter::lint(const QString& linterExecutablePath, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory)
{
    const QString debugName = "[Linter::lint] ";
    qDebug() << debugName << "Starting lint";


    // Possible reasons why lint would fail is

    // One of the inputs passed is empty or garbage
    // A non-lint executable was passed

    // Check linter version (if we can't determine the version then return error)


    // Try to generate an XML file

    return LINTER_EXECUTABLE_UNKNOWN;
    // linterExecutablePath - Path to the linter executable (including)
    // linterFilePath       - Path to the actual lint file (including)
    // linterLintOptions    - Command line options to pass to the linter
    // linterDirectory      - The directory to lint (for now)

    qDebug() << debugName << "Finished lint";

    return LINTER_OK;
}
