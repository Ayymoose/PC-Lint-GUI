#include "Linter.h"
#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include "Log.h"
#include <QFileInfo>

Linter::Linter()
{
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00a, Copyright Gimpel Software 1985-2009");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00b, Copyright Gimpel Software 1985-2009");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00c, Copyright Gimpel Software 1985-2009");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00d, Copyright Gimpel Software 1985-2009");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00e, Copyright Gimpel Software 1985-2010");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00f, Copyright Gimpel Software 1985-2010");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00g, Copyright Gimpel Software 1985-2011");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00h, Copyright Gimpel Software 1985-2011");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00i, Copyright Gimpel Software 1985-2012");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00j, Copyright Gimpel Software 1985-2012");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00k, Copyright Gimpel Software 1985-2013");
    m_supportedVersions.insert("PC-lint for C/C++ (NT) Vers. 9.00L, Copyright Gimpel Software 1985-2014");
}

LINTER_STATUS Linter::lint(const QString& linterExecutable, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory, QList<lintMessage>& lintOutputMessages)
{
    // Possible reasons why lint would fail is

    // One of the inputs passed is empty or garbage
    // A non-lint executable was passed

    QProcess lintProcess;
    qDebug() << "Setting working directory to: " << QFileInfo(linterFilePath).canonicalPath();
    Log::log("Setting working directory to: " + QFileInfo(linterFilePath).canonicalPath());
    lintProcess.setWorkingDirectory(QFileInfo(linterFilePath).canonicalPath());
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
    qDebug() << "lint arguments: " << lintArguments;

    Log::log("Lint path: " + linterExecutable);
    Log::log("Lint file: " + linterFilePath);
    Log::log("Lint directory: " + linterDirectory);
    Log::log("Lint arguments: " + lintArguments.join(""));

    lintProcess.setProgram(linterExecutable);
    lintProcess.setArguments(lintArguments);
    lintProcess.start();

    if (!lintProcess.waitForStarted())
    {
        Log::log("### Failed to start lint executable because " + lintProcess.errorString());
        return LINTER_FAIL;
    }

    QByteArray lintData;

    while(lintProcess.waitForReadyRead())
    {
        lintData.append(lintProcess.readAll());
    }

    // Remove the version information
    QByteArray lintVersion = lintData.left(lintData.indexOf("\r\n"));

    // Check linter version (if we can't determine the version then return error)
    if (!m_supportedVersions.contains(lintVersion))
    {
        Log::log("### Failed to start lint because version unsupported: " + QString(lintVersion));
        return LINTER_UNSUPPORTED_VERSION;
    }
    // Show lint version used
    Log::log("Lint version: " + QString(lintVersion));
    qDebug() << "Lint version: " + QString(lintVersion);

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
        Log::log("### Failed to parse XML because " + lintXML.errorString());
        return LINTER_ERROR;
    }


    // linterExecutablePath - Path to the linter executable (including)
    // linterFilePath       - Path to the actual lint file (including)
    // linterLintOptions    - Command line options to pass to the linter
    // linterDirectory      - The directory to lint (for now)

    lintOutputMessages = lintMessages;

    return LINTER_OK;
}

QString Linter::getLintingDirectory() const
{
    return m_lintingDirectory;
}

