#include "Linter.h"
#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include <QTranslator>
#include <QFileInfo>

// Currently only
// PC-Lint v9 is supported

LINTER_STATUS Linter::lint(const QString& linterExecutable, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory, QList<lintMessage>& lintOutputMessages)
{
    // Possible reasons why lint would fail is

    // One of the inputs passed is empty or garbage
    // A non-lint executable was passed


    // Test

    //QString test = "D:\\Users\\Ayman\\Downloads\\thief_2_service_release\\thief_2_service_release\\rdrive\\prj\\thief2\\src\\DARK\\CRCUTTY.C";


    QProcess lintProcess;

    lintProcess.setWorkingDirectory("D:/Users/Ayman/Desktop/MerlinEmbedded/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/lint/");

   // lintProcess.setWorkingDirectory(QFileInfo(linterFilePath).canonicalPath());
    QStringList lintArguments;

    // This is the extra file we must pass into the linter to produce XML output
    QStringList xmlArguments;
    xmlArguments << "-v"
                 << "-width(0)"
                 //<< "+xml(?xml version=1.0?)"
                 << "+xml(doc)"
                 << "-""format=<message><file>%f</file><line>%l</line><type>%t</type><code>%n</code><description>%m</description></message>"
                 << "-""format_specific= "
                 << "-hFs1"
                 << "-pragma(message)";

    lintProcess.setProcessChannelMode(QProcess::MergedChannels);

    // Add any options we have
    if (linterLintOptions != "")
    {
        lintArguments << linterLintOptions;
    }

    // Lint only C files for now
    lintArguments << xmlArguments << linterFilePath << (linterDirectory + "\\*.c");

    qDebug() << "lint path: " << linterExecutable;
    qDebug() << "lint file: " << linterFilePath;
    qDebug() << "lint directory: " << linterDirectory;
    qDebug() << "";
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

    // Remove the version information
    QByteArray lintVersion = lintData.left(QString(LINT_VERSION).length());

    // Check linter version (if we can't determine the version then return error)
    if (lintVersion != LINT_VERSION)
    {
        return LINTER_UNSUPPORTED_VERSION;
    }

    // Remove version information
    lintData.remove(0,lintVersion.length());
    qDebug() << lintData.data();


    QList<lintMessage> lintMessages;
    QXmlStreamReader lintXML(lintData);
    lintMessage message;

    // Start XML parsing
    //Parse the XML until we reach end of it
    while(!lintXML.atEnd() && !lintXML.hasError())
    {

        // Read next element
        QXmlStreamReader::TokenType token = lintXML.readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement)
        {
            if(lintXML.name() == XML_DOC || lintXML.name() == XML_MESSAGE)
            {
                continue;
            }

            if(lintXML.name() == XML_FILE)
            {
                message.file = lintXML.readElementText();
            }
            if(lintXML.name() == XML_LINE)
            {
                message.line = lintXML.readElementText();
            }
            if(lintXML.name() == XML_TYPE)
            {
                message.type = lintXML.readElementText();
            }
            if(lintXML.name() == XML_CODE)
            {
                message.code = lintXML.readElementText();
            }
            if(lintXML.name() == XML_DESCRIPTION)
            {
                message.description = lintXML.readElementText();
            }
        }

        if((token == QXmlStreamReader::EndElement) && (lintXML.name() == XML_MESSAGE))
        {
            lintMessages.append(message);
            message = {};
        }
    }

    if(lintXML.hasError())
    {
        qDebug() << lintXML.errorString();
        return LINTER_ERROR;
    }


    // linterExecutablePath - Path to the linter executable (including)
    // linterFilePath       - Path to the actual lint file (including)
    // linterLintOptions    - Command line options to pass to the linter
    // linterDirectory      - The directory to lint (for now)

    lintOutputMessages = lintMessages;

    return LINTER_OK;
}
