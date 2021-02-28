// PC-Lint GUI
// Copyright (C) 2021  Ayymooose

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Linter.h"
#include "Log.h"

#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QMessageBox>
#include <windows.h>
#include <QRegularExpression>
#include <chrono>
#include <QThread>
#include <vector>
#include <algorithm>
#include <QDir>

namespace Lint
{

Linter::Linter() : m_numberOfErrors(0), m_numberOfWarnings(0), m_numberOfInfo(0)
{
}

LintMessages Linter::getLinterMessages() const noexcept
{
    return m_linterMessages;
}

void Linter::setLintFile(const QString& lintFile) noexcept
{
    m_lintFile = lintFile;
}

void Linter::setLintExecutable(const QString& linterExecutable) noexcept
{
    m_linterExecutable = linterExecutable;
}

void Linter::setSourceFiles(const QSet<QString>& files) noexcept
{
    m_filesToLint = files;
}

int Linter::numberOfInfo() const noexcept
{
    return m_numberOfInfo;
}

int Linter::numberOfWarnings() const noexcept
{
    return m_numberOfWarnings;
}

int Linter::numberOfErrors() const noexcept
{
    return m_numberOfErrors;
}

void Linter::slotGetLinterData(const LintData& lintData) noexcept
{
    qDebug().noquote() << '[' << QThread::currentThreadId() << "] - Got linter data";
    setSourceFiles(lintData.lintFiles);
    setLintFile(lintData.lintOptionFile);
    setLintExecutable(lintData.linterExecutable);
}

void Linter::setLinterMessages(const LintMessages& lintMessages) noexcept
{
    m_linterMessages = lintMessages;
}

void Linter::setNumberOfErrors(int numberOfErrors) noexcept
{
    m_numberOfErrors = numberOfErrors;
}

void Linter::setNumberOfWarnings(int numberOfWarnings) noexcept
{
    m_numberOfWarnings = numberOfWarnings;
}

void Linter::setNumberOfInfo(int numberOfInfo) noexcept
{
    m_numberOfInfo = numberOfInfo;
}

void Linter::slotStartLint() noexcept
{
    LintResponse lintResponse;
    lintResponse.status = lint();
    lintResponse.lintMessages = m_linterMessages;
    lintResponse.numberOfErrors = m_numberOfErrors;
    lintResponse.numberOfWarnings = m_numberOfWarnings;
    lintResponse.numberOfInfo = m_numberOfInfo;
    lintResponse.errorMessage = m_linterErrorMessage;
    emit signalLintFinished(lintResponse);
}

void Linter::setErrorMessage(const QString& errorMessage) noexcept
{
    m_linterErrorMessage = errorMessage;
}

QString Linter::errorMessage() const noexcept
{
    return m_linterErrorMessage;
}

Lint::Status Linter::lint() noexcept
{
    Lint::Status status = Lint::LINTER_COMPLETE;

    auto startLintTime = std::chrono::steady_clock::now();

    Q_ASSERT(m_lintFile.length());
    qDebug().noquote() << '[' << QThread::currentThreadId() << "] Setting working directory to: " << QFileInfo(m_lintFile).canonicalPath();

    QProcess lintProcess;
    lintProcess.setWorkingDirectory(QFileInfo(m_lintFile).canonicalPath());

    // stderr has the module (file lint) progress
    // sttout has the actual data
    QByteArray lintData;


    QString cmdString = m_linterExecutable;

    QObject::connect(&lintProcess, &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        if (status != Lint::LINTER_ABORT && status != Lint::LINTER_UNSUPPORTED_VERSION)
        {
            status = Lint::LINTER_PROCESS_ERROR;
            qCritical() << "Process error:" << error;
            qCritical() << "Process state:" << lintProcess.state();
        }
    });

    QObject::connect(&lintProcess, &QProcess::readyReadStandardOutput, this, [&]()
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            status = Lint::LINTER_ABORT;
            lintProcess.closeReadChannel(QProcess::StandardOutput);
            lintProcess.closeReadChannel(QProcess::StandardError);
            lintProcess.close();
            return;
        }

        QByteArray readStdOut = lintProcess.readAllStandardOutput();
        lintData.append(readStdOut);
    });

    QList<QString> lintedFiles;
    QByteArray pclintVersion;

    int processedFiles = 0;
    int totalFiles = m_filesToLint.size();
    std::chrono::steady_clock::time_point lintStartTime;
    float processingTimeTotal = 0;


    QObject::connect(&lintProcess, &QProcess::started, this, [&]()
    {
        lintStartTime = std::chrono::steady_clock::now();
    });

    QObject::connect(&lintProcess, &QProcess::readyReadStandardError, this,[&,this]()
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            status = Lint::LINTER_ABORT;
            lintProcess.closeReadChannel(QProcess::StandardOutput);
            lintProcess.closeReadChannel(QProcess::StandardError);
            lintProcess.close();
            return;
        }

        QByteArray readStdErr = lintProcess.readAllStandardError();

        // Example of module information received:
        //
        // "\r\n--- Module:   D:\\Users\\Ayman\\Desktop\\MerlinEmbedded\\Buhler Sortex Shared Code\\ATxmega\\Camera\\Cam_debug.c (C)\r\n"
        // "\r\n--- Module:   D:\\Users\\Ayman\\Desktop\\MerlinEmbedded\\Buhler Sortex Shared Code\\ATxmega\\Camera\\Cam_debug.c (CPP)\r\n"

        QRegularExpression fileRegularExpression("([A-Za-z]:\\\\(?:[^\\\\/:*?\"<>|\r\n]+\\\\)*[^\\\\/:*?\"<>|\r\n]*)");
        QRegularExpressionMatchIterator it = fileRegularExpression.globalMatch(readStdErr);

        // Get the file name + path by trimming the string
        if (pclintVersion.length() == 0)
        {
            // PC-Lint version is always the first line included in stderr
            int versionEnd = readStdErr.indexOf("\r\n");
            pclintVersion = readStdErr.left(versionEnd);

            // Some unknown executable or lint version we don't know about
            if (pclintVersion.contains("License Error"))
            {
                status = Lint::LINTER_LICENSE_ERROR;
                lintProcess.closeReadChannel(QProcess::StandardOutput);
                lintProcess.closeReadChannel(QProcess::StandardError);
                qCritical().noquote() << "Failed to start lint because of license error:\n" << readStdErr;
                m_linterErrorMessage = readStdErr;
                lintProcess.close();
                return;
            }

            if (!(pclintVersion.contains("PC-lint Plus") || pclintVersion.contains("PC-lint")))
            {
                status = Lint::LINTER_UNSUPPORTED_VERSION;
                lintProcess.closeReadChannel(QProcess::StandardOutput);
                lintProcess.closeReadChannel(QProcess::StandardError);
                qCritical().noquote() << "Failed to start lint because version unsupported: " << QString(pclintVersion);
                lintProcess.close();
                return;
            }

            // But it sometimes drags module information into the first read
            // Find any module information that sneaked into this chunk
            while (it.hasNext())
            {
                QRegularExpressionMatch match = it.next();
                QString file = match.captured(0);
                // -4 is to get rid of the " (C)" that the matcher thinks is part of the file name
                // But it's something lint adds to the end for some reason
                file = file.left(file.length()-4);
                if (!lintedFiles.contains(file))
                {
                    qDebug().noquote() << '[' << QThread::currentThreadId() << "] Processed chunk: " << file;
                    emit signalUpdateProgress(1);
                    processedFiles++;
                    lintedFiles.append(file);
                }
            }
        }
        else if (readStdErr.length() > 0)
        {
            int nowProcessedFiles = processedFiles;
            while (it.hasNext())
            {
                QRegularExpressionMatch match = it.next();
                QString file = match.captured(0);
                file = file.left(file.length()-4);

                // Linter can perform multiple passes over the same file
                // TODO: Support multiple passes
                if (!lintedFiles.contains(file))
                {
                    qDebug() << '[' << QThread::currentThreadId() << "] Processed: " << file;
                    emit signalUpdateProgress(1);
                    processedFiles++;
                    emit signalUpdateProcessedFiles(1);
                    lintedFiles.append(file);
                }
            }

            // TODO: Fix ETA unreliability
            // TODO: Throw useful error messages

            //if (processingTimeTotal == 0)
            {
                // In case we process more than 1 file in a single read of stderr
                int newProcessedFiles = (processedFiles - nowProcessedFiles);

                // If there are any new processed files
                if (newProcessedFiles != 0)
                {
                    std::chrono::steady_clock::time_point lintEndTime = std::chrono::steady_clock::now();
                    // Total processing time (in ms) for newProcessedFiles files
                    processingTimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(lintEndTime - lintStartTime).count();

                    // Processing time for 1 file
                    float processingTime = (processingTimeTotal / 1000.0f) / newProcessedFiles;
                    qDebug() << '[' << QThread::currentThreadId() << "] Processed: " << newProcessedFiles << " file in " << (processingTimeTotal / 1000.0f) << "s";
                    float eta = 1 + (processingTime * (m_filesToLint.size() - processedFiles));
                    qDebug() << '[' << QThread::currentThreadId() << "] ETA: " << eta << "s";
                    lintStartTime = std::chrono::steady_clock::now();

                    static float etaMax = 0;
                    if (etaMax < eta)
                    {
                        etaMax = eta;
                        emit signalUpdateETA(eta);
                    }
                }



            }


        }

    });

    // Clear existing arguments
    m_arguments.clear();

    // Extra arguments to produce XML output
    // Enable verbosity and module information displayed so we know progress
    m_arguments << ("+vm");
    // Control the message height and force file information per message
    m_arguments << ("-hFs1"); // was b
    // No line breaks in output
    m_arguments << ("-width(0)");

    // XML specific arguments
    // Put open/closing <doc> tags in document
    m_arguments << ("+xml(doc)");
    // Use minimal element names for faster output
    m_arguments << ("-format=<m><f>%f</f><l>%l</l><t>%t</t><n>%n</n><d>%m</d></m>");
    // Surpress specific walk messages
    m_arguments << ("-format_specific= ");

    // TODO: Test with various lint files

    // TODO: Support PC-Lint Plus
    // PC-Lint Plus argument
    //m_arguments << ("-max_threads=8");

     // warning -> supplemental
     // info -> supplemental

   // m_arguments << ("-passes(6)");

    cmdString += " \"" + m_lintFile + "\"";

    for (const auto& str : m_arguments)
    {
        cmdString += " \"" + str + "\"";
    }

    // Add the lint file
    m_arguments << (m_lintFile);

    // Assert argument length + lint executable path < 512
    int totalLength = 0;
    for (const auto& argument : m_arguments)
    {
        totalLength += argument.length();
    }

    Q_ASSERT(totalLength + m_linterExecutable.length() < MAX_LINT_PATH);

    // Add all files to lint    
    for (const auto& file : m_filesToLint)
    {
       m_arguments << file;
       cmdString += " \"" + file + "\"";
       qInfo().noquote() << "Adding file to lint: " << file;
    }

    Q_ASSERT(m_linterExecutable.length());
    qInfo().noquote() << "Lint path: " << m_linterExecutable;
    qInfo().noquote() << "Lint file: " << m_lintFile;

    // TODO: Temporary debug information
    QString thisFileString = "D:\\Users\\Ayman\\Desktop\\PC-Lint GUI\\test\\cmdline";
    thisFileString += QString("0x%1").arg((quintptr)QThread::currentThreadId(),
                        QT_POINTER_SIZE * 2, 16, QChar('0'));

    thisFileString += ".xml";

    QFile file2(thisFileString);
    file2.open(QIODevice::WriteOnly);
    Q_ASSERT(file2.isOpen());
    auto bytesWritten = file2.write(cmdString.toLocal8Bit());
    Q_ASSERT(bytesWritten > 0);
    file2.close();

    lintProcess.setProgram(m_linterExecutable);
    lintProcess.setArguments(m_arguments);
    lintProcess.start();


    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;

    if (!lintProcess.waitForStarted())
    {
        qCritical().noquote() << lintProcess.errorString();
        return Lint::LINTER_PROCESS_ERROR;
    }

    // Estimate progress of lint
    // PC-Lint gives no progress indication other than which modules were processed
    // Therefore we have to estimate how long the entire lint process can possibly take
    // A simple way to do this is by
    // Calculating how long it look to process 1 module (tM) in seconds
    // Assume each module will take the same time tM so the total time is tM * number of files
    // Update tM by averaging it over the number of processed modules

    qDebug() << '[' << QThread::currentThreadId() << "] Total files: " << totalFiles;

    emit signalUpdateProgressMax(totalFiles);
    emit signalUpdateProcessedFiles(processedFiles);

    // Wait forever until finished
    // TODO: Wait forever but timeout after 1 minute if stuck on a file?
    // TODO: Loop with a delay to check if lint process finished
    if (!lintProcess.waitForFinished())
    {
        if (status == LINTER_PROCESS_ERROR)
        {
            return status;
        }
        else
        {
            qCritical().noquote() << "Lint process exited as it took too long: " << QString::number(Lint::MAX_LINT_TIME) << "ms. It's possible the lint executable became stuck on a particular file";
            lintProcess.close();
            return Lint::LINTER_PROCESS_TIMEOUT;
        }
    }

    // TODO: Command string
    // BUG: Gets stuck
    // D:/lint/lint-nt.exe "+vm" "-hFs1" "-width(0)" "+xml(doc)" "-format=<m><f>%f</f><l>%l</l><t>%t</t><n>%n</n><d>%m</d></m>" "-format_specific= " "D:/Users/Ayman/Desktop/CA20-6011-Bichro-SC-Application-Common-PCM-9310-Windows10/Source/bichro_sc.lnt" "D:/Users/Ayman/Desktop/CA20-6011-Bichro-SC-Application-Common-PCM-9310-Windows10/Source/GUI_dll/DLG_Settings_FrameCapture.cpp"



    // Check linter version (if we can't determine the version then return error)
    if (status == Lint::LINTER_ABORT || status == Lint::LINTER_LICENSE_ERROR || status == Lint::LINTER_UNSUPPORTED_VERSION)
    {
        return status;
    }

    if (lintedFiles.size() == m_filesToLint.size())
    {
        qDebug().noquote() << '[' << QThread::currentThreadId() << "] All files in project linted";
    }
    else
    {
        qDebug().noquote() << '[' << QThread::currentThreadId() << "] Only " << lintedFiles.size() << '/' << m_filesToLint.size() << " were linted!";
        status = Lint::Status::LINTER_PARTIAL_COMPLETE;
    }

    // Show lint version used
    qDebug() << '[' << QThread::currentThreadId() << "] Lint version: " << QString(pclintVersion);

    lintData.replace("<doc>","");
    lintData.replace("</doc>","");
    lintData.prepend("<doc>");
    lintData.append("</doc>");
    QString lintDataAsString = QString(lintData);

    // For PC-Lint, it generates useful "wrap-up messages" but provides no way to wrap them in the XML tags
    // So we must manually do that here

    // Method 1
    // Split byte array into qlist of byte array by \n
    // For each line, if not <doc> </doc> or <f> then escape it
    // Append to string



    qDebug().noquote() << '[' << QThread::currentThreadId() << "] XML data size: " << lintData.size();


    QFile lintXMLOutputFile("D:\\Users\\Ayman\\Desktop\\PC-Lint GUI\\test\\xmldata.xml");
    lintXMLOutputFile.open(QIODevice::WriteOnly);
    lintXMLOutputFile.write(lintData);
    lintXMLOutputFile.close();
    //

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

    /*QByteArray lintData;
    QFile lintXMLOutputFile("D:\\Users\\Ayman\\Desktop\\PC-Lint GUI\\test\\xmldata.xml");
    lintXMLOutputFile.open(QIODevice::ReadOnly);
    Q_ASSERT(lintXMLOutputFile.isOpen());
    lintData = lintXMLOutputFile.readAll();
    lintXMLOutputFile.close();*/

    // Ordering of messages is now important (was QSet)
    // To group supplemental messages together (PC-Lint Plus)
    LintMessages lintMessages;

    QXmlStreamReader lintXML(lintData);
    LintMessage message;

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
            // <doc> or <m> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_DOC || lintXML.name() == Lint::Xml::XML_ELEMENT_MESSAGE)
            {
                continue;
            }

            // <f> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_FILE)
            {
                message.file = lintXML.readElementText();
                // Why does PC-Lint Plus mess with the directory separator?
                // It spits out '/' and '\' in the same path
                message.file = QDir::toNativeSeparators(message.file);
            }

            // <l> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_LINE)
            {
                message.line = lintXML.readElementText();
                Q_ASSERT(message.line.length() > 0);
            }

            // <t> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_MESSAGE_TYPE)
            {
                message.type = lintXML.readElementText();
                Q_ASSERT(message.type.length() > 0);
            }

            // <n> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_MESSAGE_NUMBER)
            {
                message.number = lintXML.readElementText();
                Q_ASSERT(message.number.length() > 0);
            }

            // <d> tag
            if(lintXML.name() == Lint::Xml::XML_ELEMENT_DESCRIPTION)
            {
                message.description = lintXML.readElementText();
                Q_ASSERT(message.description.length() > 0);
            }

        }

        if((token == QXmlStreamReader::EndElement) && (lintXML.name() == Lint::Xml::XML_ELEMENT_MESSAGE))
        {
            // Lint can spit out duplicates for some reason
            // Just add it for now

            auto contains = std::find(lintMessages.begin(), lintMessages.end(), message);
            if (contains == lintMessages.end())
            {
                lintMessages.emplace_back(message);

                // Ascertain type
                if (!QString::compare(message.type, Lint::Type::LINT_TYPE_ERROR, Qt::CaseInsensitive))
                {
                    m_numberOfErrors++;
                }
                else if (!QString::compare(message.type, Lint::Type::LINT_TYPE_WARNING, Qt::CaseInsensitive))
                {
                    m_numberOfWarnings++;
                }
                else if (!QString::compare(message.type, Lint::Type::LINT_TYPE_INFO, Qt::CaseInsensitive))
                {
                    m_numberOfInfo++;
                }
                else if (!QString::compare(message.type, Lint::Type::LINT_TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
                {
                    // Don't care about number of supplementals
                }
                else
                {
                    // Unknown types are treated as informational messages with '?' icon
                    qWarning() << "Unknown message type received: " << message.type;
                }
            }

            message = {};
        }
    }

    if (lintXML.hasError())
    {
        qCritical() << "XML parser error";
        qCritical() << "Error Type:       " << QString(lintXML.error());
        qCritical() << "Error String:     " << lintXML.errorString();
        qCritical() << "Line Number:      " << QString::number(lintXML.lineNumber());
        qCritical() << "Column Number:    " << QString::number(lintXML.columnNumber());
        qCritical() << "Character Offset: " << QString::number(lintXML.characterOffset());
        return Lint::Status::LINTER_PROCESS_ERROR;
    }


    m_linterMessages = lintMessages;

    auto endLintTime = std::chrono::steady_clock::now();
    auto totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug().noquote() << '[' << QThread::currentThreadId() << "] I took " << totalElapsedTime / 1000.f << "s" << "to complete";

    return status;
}

void Linter::removeAssociatedMessages(const QString& file) noexcept
{
    // Find the LintMessage who has the same file part
    auto it = m_linterMessages.begin();
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

void Linter::removeMessagesWithNumber(const QString& number) noexcept
{
    // Find the LintMessage who has the same code part
    auto it = m_linterMessages.begin();
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

void Linter::appendLinterMessages(const LintMessages& lintMessages) noexcept
{
    for (const auto& message : lintMessages)
    {
        m_linterMessages.emplace_back(message);
    }
}

void Linter::appendLinterErrors(int numberOfErrors) noexcept
{
    m_numberOfErrors += numberOfErrors;
}

void Linter::appendLinterWarnings(int numberOfWarnings) noexcept
{
    m_numberOfWarnings += numberOfWarnings;
}

void Linter::appendLinterInfo(int numberOfInfo) noexcept
{
    m_numberOfInfo += numberOfInfo;
}

void Linter::resetLinter() noexcept
{
    m_linterMessages.clear();
    m_numberOfErrors = 0;
    m_numberOfWarnings = 0;
    m_numberOfInfo = 0;
}

QString Linter::getLinterFile() const noexcept
{
    return m_lintFile;
}

// Group together lint messages (PC-Lint Plus only)
// So that supplemental messages are tied together with error/info/warnings
LintMessageGroup Linter::groupLinterMessages() noexcept
{
    // Spit out a LintMessageGroup
    LintMessageGroup messageGroup;

    auto firstPtr = m_linterMessages.cbegin();
    auto secondPtr = firstPtr+1;

    // Use two pointers to find groups
    // Point first one to a message
    // Point second to message+1
    // While second pointer points to a supplemental message type, increment and add
    // Point first one to second pointer+1
    // Repeat until end

    while (firstPtr != m_linterMessages.cend() && secondPtr != m_linterMessages.cend())
    {
        // First message type should never be "Supplemental"
        Q_ASSERT(firstPtr->type != Lint::Type::LINT_TYPE_SUPPLEMENTAL);

        // Add first message
        LintMessages message;
        message.emplace_back(*firstPtr);

        // Associate supplemental messages
        while (secondPtr != m_linterMessages.cend() && secondPtr->type == Lint::Type::LINT_TYPE_SUPPLEMENTAL)
        {
            message.emplace_back(*secondPtr);
            secondPtr++;
        }

        // Add this group
        messageGroup.emplace_back(message);

        // Advance pointers
        firstPtr = secondPtr++;
    }

    return messageGroup;
}

QImage Linter::associateMessageTypeWithIcon(const QString& type) noexcept
{
    Lint::Message messageType;

    // Determine type
    if (!QString::compare(type, Lint::Type::LINT_TYPE_ERROR, Qt::CaseInsensitive))
    {
        messageType = Lint::Message::MESSAGE_TYPE_ERROR;
    }
    else if (!QString::compare(type, Lint::Type::LINT_TYPE_WARNING, Qt::CaseInsensitive))
    {
        messageType = Lint::Message::MESSAGE_TYPE_WARNING;
    }
    else if (!QString::compare(type, Lint::Type::LINT_TYPE_INFO, Qt::CaseInsensitive))
    {
        messageType = Lint::Message::MESSAGE_TYPE_INFORMATION;
    }
    else if (!QString::compare(type, Lint::Type::LINT_TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
    {
        messageType = Lint::Message::MESSAGE_TYPE_SUPPLEMENTAL;
    }
    else
    {
        messageType = Lint::Message::MESSAGE_TYPE_UNKNOWN;
    }

    QImage icon;
    switch (messageType)
    {
        case Lint::Message::MESSAGE_TYPE_ERROR: icon.load(":/images/error.png");  break;
        case Lint::Message::MESSAGE_TYPE_WARNING: icon.load(":/images/warning.png"); break;
        case Lint::Message::MESSAGE_TYPE_INFORMATION:
        case Lint::Message::MESSAGE_TYPE_SUPPLEMENTAL: icon.load(":/images/info.png"); break;
        default: Q_ASSERT(false); break;
    }

    return icon;
}


};

