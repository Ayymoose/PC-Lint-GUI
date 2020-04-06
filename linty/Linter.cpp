#include "Linter.h"
#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include "Log.h"
#include <QFileInfo>
#include <windows.h>
#include <QMessageBox>

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
    m_supportedVersions.insert("PC-lint Plus 1.3 for Windows, Copyright Gimpel Software LLC 1985-2019");
}

void Linter::addArgument(QString argument)
{
    m_arguments << argument;
}

void Linter::setLinterFile(const QString& lintFile)
{
    m_lintFile = lintFile;
}

void Linter::setLinterExecutable(const QString& linterExecutable)
{
    m_linterExecutable = linterExecutable;
}

void Linter::setLintFiles(const QList<QString>& files)
{
    m_filesToLint = files;
}

LINTER_STATUS Linter::lint(QSet<lintMessage>& lintOutputMessages)
{

    DEBUG_LOG("Setting working directory to: " + QFileInfo(m_lintFile).canonicalPath());

    QProcess lintProcess;
    lintProcess.setWorkingDirectory(QFileInfo(m_lintFile).canonicalPath());
    lintProcess.setProcessChannelMode(QProcess::MergedChannels);

    // Clear existing arguments
    m_arguments.clear();

    // Extra arguments to produce XML output
    addArgument("-v");
    addArgument("-width(0)");
    addArgument("+xml(doc)");
    addArgument("-""format=<message><file>%f</file><line>%l</line><type>%t</type><code>%n</code><description>%m</description></message>");
    addArgument("-""format_specific= ");
    addArgument("-hFs1");
    addArgument("-pragma(message)");

    // Add the lint file
    addArgument(m_lintFile);

    // Add all files to lint
    for (const QString& file : m_filesToLint)
    {
       addArgument(file);
       DEBUG_LOG("Adding file to lint: " + file);
    }

    DEBUG_LOG("Lint path: " + m_linterExecutable);
    DEBUG_LOG("Lint file: " + m_lintFile);

    // Display arguments
    for (const QString& argument : m_arguments)
    {
        DEBUG_LOG("Lint argument: " + argument);
    }

    lintProcess.setProgram(m_linterExecutable);
    lintProcess.setArguments(m_arguments);
    lintProcess.start();

    if (!lintProcess.waitForStarted())
    {
        DEBUG_LOG("### Failed to start lint executable because " + lintProcess.errorString());
        return LINTER_FAIL;
    }

    QByteArray lintData;
    int progress = 0;
    int maxProgress = 2 * m_arguments.size();
    emit signalUpdateProgressMax(maxProgress);

    while(lintProcess.waitForReadyRead())
    {
        lintData.append(lintProcess.readAll());
        emit signalUpdateProgress(progress++);
        //qDebug() << "Progress: " << 100 * (progress/(float)maxProgress) << "%";
    }

    // Remove the version information
    QByteArray lintVersion = lintData.left(lintData.indexOf("\r\n"));

    // Check linter version (if we can't determine the version then return error)
    if (!m_supportedVersions.contains(lintVersion))
    {
        DEBUG_LOG("### Failed to start lint because version unsupported: " + QString(lintVersion));
        return LINTER_UNSUPPORTED_VERSION;
    }


    // Show lint version used
    DEBUG_LOG("Lint version: " + QString(lintVersion));

    // Remove version information
    lintData.remove(0,lintVersion.length());

    // Remove any unexpected data we may encounter after the version information
    QString lintDataAsString = QString(lintData);
    uint64_t docIndex = lintDataAsString.indexOf("<doc>");
    lintData.remove(0,docIndex);

    qDebug() << "XML data size: " << lintData.size();
    // TODO: Tidy up
    QFile file("D:\\Users\\Ayman\\Desktop\\Linty\\test\\xmldata.xml");
    file.open(QIODevice::WriteOnly);
    file.write(lintData.data());
    file.close();



    QSet<lintMessage> lintMessages;
    QXmlStreamReader lintXML(lintData);
    lintMessage message;

    progress = 0;
    maxProgress = lintData.size();
    emit signalUpdateProgress(progress);
    emit signalUpdateProgressMax(maxProgress);
    emit signalUpdateStatus("Parsing data...");

    // Start XML parsing
    // Parse the XML until we reach end of it
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
            lintMessages.insert(message);
            progress += (message.code + message.description + message.type + message.line + message.file).size();
            emit signalUpdateProgress(progress);
            message = {};
        }
        //qDebug() << "Progress: " << 100 * (progress/(float)maxProgress) << "%";
    }

    if (lintXML.hasError())
    {
        DEBUG_LOG("### Failed to parse XML because " + lintXML.errorString());
        return LINTER_ERROR;
    }


    lintOutputMessages = lintMessages;

    return LINTER_OK;
}

QString Linter::getLintingDirectory() const
{
    return m_lintingDirectory;
}

QString Linter::getLinterExecutable() const
{
    return m_linterExecutable;
}

QList<QString> AtmelStudio7ProjectSolution::buildSourceFiles(const QString& projectFileName)
{
    QList<QString> sourceFiles;

    // Get the file path for the project
    QString projectPath = QFileInfo(projectFileName).absolutePath();

    // Project
    // - ItemGroup
    // -- Compile Include
    QXmlStreamReader xmlReader;
    QFile projectFile(projectFileName);
    if (projectFile.open(QIODevice::ReadOnly))
    {
        xmlReader.setDevice(&projectFile);
        while (!xmlReader.atEnd() && !xmlReader.hasError())
        {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader.readNext();

            // If token is just StartDocument - go to next
            if (token == QXmlStreamReader::StartDocument)
            {
                continue;
            }

            // If token is StartElement - read it
            if (token == QXmlStreamReader::StartElement)
            {
                QStringRef name = xmlReader.name();
                if (name == "Compile")
                {
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    QString include = attrs.value("Include").toString();

                    if (QFileInfo(include).suffix() == "c")
                    {
                        // Append the project path so we can get the canonical file path
                        QString sourceFile = QFileInfo(projectPath + "\\" + include).canonicalFilePath();
                        sourceFiles.append(sourceFile);
                        if (sourceFile.length() >= MAX_PATH)
                        {
                            DEBUG_LOG("### Source file name longer than " + QString(MAX_PATH) + " characters");
                        }
                    }

                }
                else
                {
                    continue;
                }

            }

        }
        if(xmlReader.hasError())
        {
            qDebug() << "Error Type:       " << xmlReader.error();
            qDebug() << "Error String:     " << xmlReader.errorString();
            qDebug() << "Line Number:      " << xmlReader.lineNumber();
            qDebug() << "Column Number:    " << xmlReader.columnNumber();
            qDebug() << "Character Offset: " << xmlReader.characterOffset();
        }
    }
    else
    {
        qDebug() << "Unable to open file for reading";
    }

    return sourceFiles;
}
