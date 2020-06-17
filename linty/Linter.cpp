#include "Linter.h"
#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include "Log.h"
#include <QFileInfo>
#include <QMessageBox>
#include <windows.h>

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
    m_supportedVersions.insert("PC-lint Plus 1.3.5 for Windows, Copyright Gimpel Software LLC 1985-2020");

    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;
}

QSet<LintMessage> Linter::getLinterMessages() const
{
    return m_linterMessages;
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

int Linter::numberOfInfo() const
{
    return m_numberOfInfo;
}

int Linter::numberOfWarnings() const
{
    return m_numberOfWarnings;
}

int Linter::numberOfErrors() const
{
    return m_numberOfErrors;
}

LINTER_STATUS Linter::lint()
{
    DEBUG_LOG("Setting working directory to: " + QFileInfo(m_lintFile).canonicalPath());

    QProcess lintProcess;
    lintProcess.setWorkingDirectory(QFileInfo(m_lintFile).canonicalPath());
    lintProcess.setProcessChannelMode(QProcess::MergedChannels);

    // Clear existing arguments
    m_arguments.clear();

    // Extra arguments to produce XML output
    m_arguments << ("-v");
    m_arguments << ("-hFs1"); // was b
    m_arguments << ("-width(0)");
    m_arguments << ("+xml(doc)");
    m_arguments << ("-format=<m><f>%f</f><l>%l</l><t>%t</t><n>%n</n><d>%m</d></m>");
    m_arguments << ("-format_specific= ");

    // PC-Lint Plus argument
    //m_arguments << ("-max_threads=8");



    /*
     *
     * warning -> supplemental
     * info -> supplemental
     *
     */

    // Add the lint file
    m_arguments << (m_lintFile);

    // Add all files to lint
    for (const QString& file : m_filesToLint)
    {
       if (file.length() > MAX_PATH)
       {
           Q_ASSERT(false);
       }
       m_arguments << (file);
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

    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;

    if (!lintProcess.waitForStarted())
    {
        DEBUG_LOG("### " + lintProcess.errorString());
        return LINTER_FAIL;
    }

    // TODO: Improve
    // Estimate progress of lint
    QByteArray lintData;
    int progress = 0;
    int maxProgress = 2 * m_arguments.size();
    emit signalUpdateProgressMax(maxProgress);

    while(lintProcess.waitForReadyRead())
    {
        lintData.append(lintProcess.readAll());
        emit signalUpdateProgress(progress++);
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
    uint64_t docIndex = lintDataAsString.indexOf(XML_TAG_DOC_OPEN);
    lintData.remove(0,docIndex);

    // For PC-Lint, it generates useful "wrap-up messages" but provides no way to wrap them in the XML tags
    // So we must manually do that here

    // Method 1
    // Split byte array into qlist of byte array by \n
    // For each line, if not <doc> </doc> or <f> then escape it
    // Append to string

    /*QList<QByteArray> lines = lintData.split('\n');
    // The error we need to insert
    QString errorToInsert;
    // The XML message to insert into
    QString xmlMessage;

    QString xmlStringNew;
    xmlStringNew.reserve(lintDataAsString.size());
    xmlStringNew = XML_TAG_DOC_OPEN;
    xmlStringNew +="\n";

    for(const QByteArray& line : lines)
    {
        if (xmlMessage.length() > 0)
        {
            xmlMessage.insert(xmlMessage.indexOf(XML_TAG_DESCRIPTION_CLOSED), "\n" + errorToInsert);
            xmlStringNew += xmlMessage;
            xmlMessage.clear();
            errorToInsert.clear();
        }
        if (!(line.startsWith(XML_TAG_DOC_OPEN) || line.startsWith(XML_TAG_DOC_CLOSED) || line.startsWith(XML_TAG_MESSAGE_OPEN)))
        {
            // Escape the error message
            errorToInsert+= QString(line).toHtmlEscaped();
        }
        else if (line.startsWith(XML_TAG_MESSAGE_OPEN))
        {
            xmlMessage = line;
        }
    }

    xmlStringNew += "</doc>";*/

    qDebug() << "XML data size: " << lintData.size();

    // TODO: Temporary debug information
    QFile file("D:\\Users\\Ayman\\Desktop\\Linty\\test\\xmldata.xml");
    file.open(QIODevice::WriteOnly);
    file.write(lintDataAsString.toLocal8Bit().data());
    file.close();
    //

    QSet<LintMessage> lintMessages;
    QXmlStreamReader lintXML(lintDataAsString);
    LintMessage message;

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
            if(lintXML.name() == XML_ELEMENT_DOC || lintXML.name() == XML_ELEMENT_MESSAGE)
            {
                continue;
            }

            if(lintXML.name() == XML_ELEMENT_FILE)
            {
                message.file = lintXML.readElementText();
                Q_ASSERT(message.file.length() > 0);
            }
            if(lintXML.name() == XML_ELEMENT_LINE)
            {
                message.line = lintXML.readElementText();
                Q_ASSERT(message.line.length() > 0);
            }
            if(lintXML.name() == XML_ELEMENT_MESSAGE_TYPE)
            {
                message.type = lintXML.readElementText();
                Q_ASSERT(message.type.length() > 0);
            }
            if(lintXML.name() == XML_ELEMENT_MESSAGE_NUMBER)
            {
                message.number = lintXML.readElementText();
                Q_ASSERT(message.number.length() > 0);
            }
            if(lintXML.name() == XML_ELEMENT_DESCRIPTION)
            {
                message.description = lintXML.readElementText();
                Q_ASSERT(message.description.length() > 0);
            }

        }

        if((token == QXmlStreamReader::EndElement) && (lintXML.name() == XML_ELEMENT_MESSAGE))
        {
            // Lint can spit out duplicates for some reason
            // This is a quick way to check if it was inserted or not
            int size = lintMessages.size();
            lintMessages.insert(message);
            if (lintMessages.size() > size)
            {
                // Ascertain type
                if (!QString::compare(message.type, LINT_TYPE_ERROR, Qt::CaseInsensitive))
                {
                    m_numberOfErrors++;
                }
                else if (!QString::compare(message.type, LINT_TYPE_WARNING, Qt::CaseInsensitive))
                {
                    m_numberOfWarnings++;
                }
                else if (!QString::compare(message.type, LINT_TYPE_INFO, Qt::CaseInsensitive))
                {
                    m_numberOfInfo++;
                }
                else
                {
                    // TODO: Fix for supplemental messages
                    // Unknown types are treated as informational messages with '?' icon
                    m_numberOfInfo++;
                }
            }
            progress += (message.number + message.description + message.type + message.line + message.file).size();
            emit signalUpdateProgress(progress);
            message = {};
        }
    }

    if (lintXML.hasError())
    {
        DEBUG_LOG("### XML parser error");
        DEBUG_LOG("Error Type:       " + QString(lintXML.error()));
        DEBUG_LOG("Error String:     " + lintXML.errorString());
        DEBUG_LOG("Line Number:      " + QString::number(lintXML.lineNumber()));
        DEBUG_LOG("Column Number:    " + QString::number(lintXML.columnNumber()));
        DEBUG_LOG("Character Offset: " + QString::number(lintXML.characterOffset()));
        return LINTER_ERROR;
    }


    m_linterMessages = lintMessages;

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

void Linter::removeAssociatedMessages(const QString& file)
{
    // Find the LintMessage who has the same file part
    QSet<LintMessage>::iterator it = m_linterMessages.begin();
    while (it != m_linterMessages.end())
    {
        if ((*it).file == file)
        {
            // Remove this message
            it = m_linterMessages.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Linter::removeMessagesWithNumber(const QString& number)
{
    // Find the LintMessage who has the same code part
    QSet<LintMessage>::iterator it = m_linterMessages.begin();
    while (it != m_linterMessages.end())
    {
        if ((*it).number == number)
        {
            // Remove this message
            it = m_linterMessages.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
