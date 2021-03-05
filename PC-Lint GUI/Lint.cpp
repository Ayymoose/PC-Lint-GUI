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

#include "Lint.h"
#include "Log.h"

#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <chrono>
#include <QThread>
#include <vector>
#include <algorithm>
#include <QDir>

namespace PCLint
{

Lint::Lint() :
    m_numberOfErrors(0),
    m_numberOfWarnings(0),
    m_numberOfInfo(0),
    m_version(VERSION_UNKNOWN),
    m_status(STATUS_UNKNOWN),
    m_numberOfLintedFiles(0)
{
}

LintMessages Lint::getLintMessages() const noexcept
{
    return m_messages;
}

void Lint::setLintFile(const QString& lintFile) noexcept
{
    m_lintFile = lintFile;
}

void Lint::setLintExecutable(const QString& linterExecutable) noexcept
{
    m_lintExecutable = linterExecutable;
}

void Lint::setSourceFiles(const QSet<QString>& files) noexcept
{
    m_filesToLint = files;
}

int Lint::numberOfInfo() const noexcept
{
    return m_numberOfInfo;
}

int Lint::numberOfWarnings() const noexcept
{
    return m_numberOfWarnings;
}

int Lint::numberOfErrors() const noexcept
{
    return m_numberOfErrors;
}

void Lint::slotGetLintData(const LintData& lintData) noexcept
{
    qDebug() << '[' << QThread::currentThreadId() << "] - Got linter data";
    setSourceFiles(lintData.lintFiles);
    setLintFile(lintData.lintOptionFile);
    setLintExecutable(lintData.linterExecutable);
}

void Lint::setLintData(const LintData& lintData) noexcept
{
    Q_ASSERT(lintData.lintFiles.size());
    Q_ASSERT(lintData.lintOptionFile.size());
    Q_ASSERT(lintData.linterExecutable.size());

    m_filesToLint = lintData.lintFiles;
    m_lintFile = lintData.lintOptionFile;
    m_lintExecutable = lintData.linterExecutable;
}


void Lint::setLintMessages(const LintMessages& lintMessages) noexcept
{
    m_messages = lintMessages;
}

void Lint::setNumberOfErrors(int numberOfErrors) noexcept
{
    m_numberOfErrors = numberOfErrors;
}

void Lint::setNumberOfWarnings(int numberOfWarnings) noexcept
{
    m_numberOfWarnings = numberOfWarnings;
}

void Lint::setNumberOfInfo(int numberOfInfo) noexcept
{
    m_numberOfInfo = numberOfInfo;
}

void Lint::setErrorMessage(const QString& errorMessage) noexcept
{
    m_errorMessage = errorMessage;
}

QString Lint::errorMessage() const noexcept
{
    return m_errorMessage;
}

void Lint::asyncLint() noexcept
{
    Q_ASSERT(m_filesToLint.size());
    Q_ASSERT(m_lintFile.size());
    Q_ASSERT(m_lintExecutable.size());

    auto const workingDirectory = QFileInfo(m_lintFile).canonicalPath();
    qDebug() << "Setting working directory to: " << workingDirectory;
    m_process.setWorkingDirectory(workingDirectory);

    // stderr has the module (file lint) progress
    // sttout has the actual data
    QString cmdString = m_lintExecutable;

    //int processedFiles = 0;
    //int totalFiles = m_filesToLint.size();
    //float processingTimeTotal = 0;

    // Reset
    m_arguments.clear();
    m_status = STATUS_UNKNOWN;
    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;
    m_numberOfLintedFiles = 0;
    m_stdOutData.clear();

    // TODO: Use 1 only and create multiple lints as threads because PCLP doesn't seem to output the file names
    // of all the files if -max_threads is set > 1
    // Has to be the first argument for it to take effect
    //if (m_version == VERSION_PC_LINT_PLUS)
    {
        m_arguments << ("-max_threads=1");
    }

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



    // TODO: Support multiple passes ("-passes(6)");
    // TODO: Support -env_push
    // TODO: Test with various lint files

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

    Q_ASSERT(totalLength + m_lintExecutable.length() < MAX_LINT_PATH);

    // Add all files to lint
    for (const auto& file : m_filesToLint)
    {
       m_arguments << file;
       cmdString += " \"" + file + "\"";
       qInfo() << "Adding file: " << file;
    }

    qInfo() << "Lint path: " << m_lintExecutable;
    qInfo() << "Lint file: " << m_lintFile;

    // Temporary debug information
    QFile commandFileDebug(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\cmdline.xml)");
    commandFileDebug.open(QIODevice::WriteOnly);
    Q_ASSERT(commandFileDebug.isOpen());
    auto bytesWritten = commandFileDebug.write(cmdString.toLocal8Bit());
    Q_ASSERT(bytesWritten > 0);
    commandFileDebug.close();

    //std::chrono::steady_clock::time_point lintStartTime;

    m_process.setProgram(m_lintExecutable);
    m_process.setArguments(m_arguments);
    m_process.start();

    // No local variables beyond this point

    QObject::connect(&m_process, &QProcess::started, this, [this]()
    {
        //lintStartTime = std::chrono::steady_clock::now();
        // Tell ProgressWindow the maximum number of files we have
        emit signalUpdateProgressMax(m_filesToLint.size());
        //emit signalUpdateProcessedFiles(m_numberOfLintedFiles);
    });

    QObject::connect(&m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
    [this](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "Lint process finished with exit code: " << QString::number(exitCode) << " and exit status: " << exitStatus;

        if (m_status & (STATUS_PROCESS_ERROR | STATUS_PROCESS_TIMEOUT | STATUS_ABORT | STATUS_LICENSE_ERROR))
        {
            emitLintComplete();
            return;
        }

        // if -env_push is used we'll lint outside files
        if (m_numberOfLintedFiles >= m_filesToLint.size())
        {
            qDebug() << "Successfully linted all files";
            m_status = STATUS_COMPLETE;
        }
        else
        {
            qDebug() << "Linted only " << m_numberOfLintedFiles << '/' << m_filesToLint.size() << "files";
            m_status = STATUS_PARTIAL_COMPLETE;
        }

        // Fix XML doc tags that are littered about
        m_stdOutData.replace(Xml::XML_TAG_DOC_OPEN,"");
        m_stdOutData.replace(Xml::XML_TAG_DOC_CLOSED,"");
        m_stdOutData.prepend("<doc>");
        m_stdOutData.append(Xml::XML_TAG_DOC_CLOSED);

        // Ordering of messages is now important (was QSet)
        // To group supplemental messages together (PC-Lint Plus)
        LintMessages lintMessages;

        QXmlStreamReader lintXML(m_stdOutData);
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
                if(lintXML.name() == Xml::XML_ELEMENT_DOC || lintXML.name() == Xml::XML_ELEMENT_MESSAGE)
                {
                    continue;
                }

                // <f> tag
                if(lintXML.name() == Xml::XML_ELEMENT_FILE)
                {
                    message.file = lintXML.readElementText();
                    // Why does PC-Lint Plus mess with the directory separator?
                    // It spits out '/' and '\' in the same path
                    message.file = QDir::toNativeSeparators(message.file);
                }

                // <l> tag
                if(lintXML.name() == Xml::XML_ELEMENT_LINE)
                {
                    message.line = lintXML.readElementText();
                    Q_ASSERT(message.line.length() > 0);
                }

                // <t> tag
                if(lintXML.name() == Xml::XML_ELEMENT_MESSAGE_TYPE)
                {
                    message.type = lintXML.readElementText();
                    Q_ASSERT(message.type.length() > 0);
                }

                // <n> tag
                if(lintXML.name() == Xml::XML_ELEMENT_MESSAGE_NUMBER)
                {
                    message.number = lintXML.readElementText();
                    Q_ASSERT(message.number.length() > 0);
                }

                // <d> tag
                if(lintXML.name() == Xml::XML_ELEMENT_DESCRIPTION)
                {
                    message.description = lintXML.readElementText();
                    Q_ASSERT(message.description.length() > 0);
                }

            }

            if((token == QXmlStreamReader::EndElement) && (lintXML.name() == Xml::XML_ELEMENT_MESSAGE))
            {
                // Lint can spit out duplicates for some reason
                // Just add it for now

                auto contains = std::find(lintMessages.begin(), lintMessages.end(), message);
                if (contains == lintMessages.end())
                {
                    lintMessages.emplace_back(message);

                    // Ascertain type
                    if (!QString::compare(message.type, Type::TYPE_ERROR, Qt::CaseInsensitive))
                    {
                        m_numberOfErrors++;
                    }
                    else if (!QString::compare(message.type, Type::TYPE_WARNING, Qt::CaseInsensitive))
                    {
                        m_numberOfWarnings++;
                    }
                    else if (!QString::compare(message.type, Type::TYPE_INFO, Qt::CaseInsensitive))
                    {
                        m_numberOfInfo++;
                    }
                    else if (!QString::compare(message.type, Type::TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
                    {
                        // Don't care about number of supplementals
                    }
                    else
                    {
                        // Unknown types are treated as informational messages with '?' icon
                        qWarning() << "Unknown message type: " << message.type;
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
            m_status = STATUS_PROCESS_ERROR;
            emitLintComplete();
            return;
        }

        m_messages = lintMessages;

        emitLintComplete();

    });

    // TODO: Use multiple threads if the user specifies if we are linting a single project (PC-Lint Plus)
    // Or need some kind of balancing algorithm?

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        qWarning() << __FUNCTION__ << " error occurred: " << error;
        m_status = STATUS_PROCESS_ERROR;
    });

    QObject::connect(&m_process, &QProcess::readyReadStandardOutput, this, [this]()
    {
        m_stdOutData.append(m_process.readAllStandardOutput());
    });

    QObject::connect(&m_process, &QProcess::readyReadStandardError, this,[this]()
    {
        auto stdErrData = m_process.readAllStandardError();

        // Check if license is valid
        // PC-Lint version is always the first line included in stderr
        if (stdErrData.contains("License Error"))
        {
            m_status = STATUS_LICENSE_ERROR;
            m_process.closeReadChannel(QProcess::StandardOutput);
            m_process.closeReadChannel(QProcess::StandardError);
            qCritical() << "Lint failed with license error:\n" << stdErrData;
            m_errorMessage = stdErrData;
            m_process.close();
            return;
        }

        QRegularExpression fileRegularExpression("([A-Za-z]:\\\\(?:[^\\\\/:*?\"<>|\r\n]+\\\\)*[^\\\\/:*?\"<>|\r\n]*)");
        auto it = fileRegularExpression.globalMatch(stdErrData);

        while (it.hasNext())
        {
            auto const match = it.next();
            auto file = match.captured(0);

            // To get the file name, remove the extension lint adds; (C++) or (C)
            if (file.endsWith(" (C++)"))
            {
                file.chop(QString(" (C++)").length());
            }
            else if (file.endsWith(" (C)"))
            {
                file.chop(QString(" (C)").length());
            }

            Q_ASSERT(QFileInfo(file).exists());

            qInfo() << "Linted:" << file;
            m_numberOfLintedFiles++;

            // Update ProgressWindow
            emit signalUpdateProgress(1);
            emit signalUpdateProcessedFiles(1);
        }

    });


}

void Lint::emitLintComplete() noexcept
{
    emit signalLintComplete(LintResponse
    {
        m_status,
        m_messages,
        m_numberOfErrors,
        m_numberOfWarnings,
        m_numberOfInfo,
        m_errorMessage
    });
}

LintStatus Lint::lint() noexcept
{
    LintStatus status = STATUS_COMPLETE;

    auto startLintTime = std::chrono::steady_clock::now();

    Q_ASSERT(m_lintFile.length());
    qDebug() << '[' << QThread::currentThreadId() << "] Setting working directory to: " << QFileInfo(m_lintFile).canonicalPath();

    m_process.setWorkingDirectory(QFileInfo(m_lintFile).canonicalPath());

    // stderr has the module (file lint) progress
    // sttout has the actual data
    QByteArray lintData;


    QString cmdString = m_lintExecutable;

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        if (status != STATUS_ABORT)
        {
            status = STATUS_PROCESS_ERROR;
            qCritical() << "Process error:" << error;
            qCritical() << "Process state:" << m_process.state();
        }
    });

    QObject::connect(&m_process, &QProcess::readyReadStandardOutput, this, [&]()
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            status = STATUS_ABORT;
            m_process.closeReadChannel(QProcess::StandardOutput);
            m_process.closeReadChannel(QProcess::StandardError);
            m_process.close();
            return;
        }

        QByteArray readStdOut = m_process.readAllStandardOutput();
        lintData.append(readStdOut);
    });

    QList<QString> lintedFiles;
    QByteArray pclintVersion;

    int processedFiles = 0;
    int totalFiles = m_filesToLint.size();
    std::chrono::steady_clock::time_point lintStartTime;
    float processingTimeTotal = 0;


    QObject::connect(&m_process, &QProcess::started, this, [&]()
    {
        lintStartTime = std::chrono::steady_clock::now();
    });

    QObject::connect(&m_process, &QProcess::readyReadStandardError, this,[&,this]()
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            status = STATUS_ABORT;
            m_process.closeReadChannel(QProcess::StandardOutput);
            m_process.closeReadChannel(QProcess::StandardError);
            m_process.close();
            return;
        }

        QByteArray readStdErr = m_process.readAllStandardError();

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
                status = STATUS_LICENSE_ERROR;
                m_process.closeReadChannel(QProcess::StandardOutput);
                m_process.closeReadChannel(QProcess::StandardError);
                qCritical() << "Failed to start lint because of license error:\n" << readStdErr;
                m_errorMessage = readStdErr;
                m_process.close();
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
                    qDebug() << '[' << QThread::currentThreadId() << "] Processed chunk: " << file;
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

    Q_ASSERT(totalLength + m_lintExecutable.length() < MAX_LINT_PATH);

    // Add all files to lint    
    for (const auto& file : m_filesToLint)
    {
       m_arguments << file;
       cmdString += " \"" + file + "\"";
       qInfo() << "Adding file to lint: " << file;
    }

    Q_ASSERT(m_lintExecutable.length());
    qInfo() << "Lint path: " << m_lintExecutable;
    qInfo() << "Lint file: " << m_lintFile;

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

    m_process.setProgram(m_lintExecutable);
    m_process.setArguments(m_arguments);
    m_process.start();


    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;

    if (!m_process.waitForStarted(MAX_WAIT_TIME))
    {
        qCritical() << m_process.errorString();
        return STATUS_PROCESS_ERROR;
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

    // TODO: This may be the total time it takes to finish
    if (!m_process.waitForFinished())
    {
        if (status == STATUS_PROCESS_ERROR)
        {
            return status;
        }
        else
        {
            qCritical() << "Lint process exited as it took too long: " << QString::number(MAX_LINT_TIME) << "ms. It's possible the lint executable became stuck on a particular file";
            m_process.close();
            return STATUS_PROCESS_TIMEOUT;
        }
    }

    // TODO: Command string
    // BUG: Gets stuck
    // D:/lint/lint-nt.exe "+vm" "-hFs1" "-width(0)" "+xml(doc)" "-format=<m><f>%f</f><l>%l</l><t>%t</t><n>%n</n><d>%m</d></m>" "-format_specific= " "D:/Users/Ayman/Desktop/CA20-6011-Bichro-SC-Application-Common-PCM-9310-Windows10/Source/bichro_sc.lnt" "D:/Users/Ayman/Desktop/CA20-6011-Bichro-SC-Application-Common-PCM-9310-Windows10/Source/GUI_dll/DLG_Settings_FrameCapture.cpp"



    // Check linter version (if we can't determine the version then return error)
    if (status == STATUS_ABORT || status == STATUS_LICENSE_ERROR)
    {
        return status;
    }


    // TODO: We can have more linted files that we put in
    if (lintedFiles.size() == m_filesToLint.size())
    {
        qDebug() << '[' << QThread::currentThreadId() << "] All files in project linted";
    }
    else
    {
        qDebug() << '[' << QThread::currentThreadId() << "] Only " << lintedFiles.size() << '/' << m_filesToLint.size() << " were linted!";
        status = LintStatus::STATUS_PARTIAL_COMPLETE;
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



    qDebug() << '[' << QThread::currentThreadId() << "] XML data size: " << lintData.size();


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
            if(lintXML.name() == Xml::XML_ELEMENT_DOC || lintXML.name() == Xml::XML_ELEMENT_MESSAGE)
            {
                continue;
            }

            // <f> tag
            if(lintXML.name() == Xml::XML_ELEMENT_FILE)
            {
                message.file = lintXML.readElementText();
                // Why does PC-Lint Plus mess with the directory separator?
                // It spits out '/' and '\' in the same path
                message.file = QDir::toNativeSeparators(message.file);
            }

            // <l> tag
            if(lintXML.name() == Xml::XML_ELEMENT_LINE)
            {
                message.line = lintXML.readElementText();
                Q_ASSERT(message.line.length() > 0);
            }

            // <t> tag
            if(lintXML.name() == Xml::XML_ELEMENT_MESSAGE_TYPE)
            {
                message.type = lintXML.readElementText();
                Q_ASSERT(message.type.length() > 0);
            }

            // <n> tag
            if(lintXML.name() == Xml::XML_ELEMENT_MESSAGE_NUMBER)
            {
                message.number = lintXML.readElementText();
                Q_ASSERT(message.number.length() > 0);
            }

            // <d> tag
            if(lintXML.name() == Xml::XML_ELEMENT_DESCRIPTION)
            {
                message.description = lintXML.readElementText();
                Q_ASSERT(message.description.length() > 0);
            }

        }

        if((token == QXmlStreamReader::EndElement) && (lintXML.name() == Xml::XML_ELEMENT_MESSAGE))
        {
            // Lint can spit out duplicates for some reason
            // Just add it for now

            auto contains = std::find(lintMessages.begin(), lintMessages.end(), message);
            if (contains == lintMessages.end())
            {
                lintMessages.emplace_back(message);

                // Ascertain type
                if (!QString::compare(message.type, Type::TYPE_ERROR, Qt::CaseInsensitive))
                {
                    m_numberOfErrors++;
                }
                else if (!QString::compare(message.type, Type::TYPE_WARNING, Qt::CaseInsensitive))
                {
                    m_numberOfWarnings++;
                }
                else if (!QString::compare(message.type, Type::TYPE_INFO, Qt::CaseInsensitive))
                {
                    m_numberOfInfo++;
                }
                else if (!QString::compare(message.type, Type::TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
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
        return LintStatus::STATUS_PROCESS_ERROR;
    }


    m_messages = lintMessages;

    auto endLintTime = std::chrono::steady_clock::now();
    auto totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug() << '[' << QThread::currentThreadId() << "] I took " << totalElapsedTime / 1000.f << "s" << "to complete";

    return status;
}

void Lint::appendLintMessages(const LintMessages& lintMessages) noexcept
{
    for (const auto& message : lintMessages)
    {
        m_messages.emplace_back(message);
    }
}

void Lint::appendLintErrors(int numberOfErrors) noexcept
{
    m_numberOfErrors += numberOfErrors;
}

void Lint::appendLintWarnings(int numberOfWarnings) noexcept
{
    m_numberOfWarnings += numberOfWarnings;
}

void Lint::appendLinterInfo(int numberOfInfo) noexcept
{
    m_numberOfInfo += numberOfInfo;
}

void Lint::resetLinter() noexcept
{
    m_messages.clear();
    m_numberOfErrors = 0;
    m_numberOfWarnings = 0;
    m_numberOfInfo = 0;
}

QString Lint::getLintFile() const noexcept
{
    return m_lintFile;
}

// Group together lint messages (PC-Lint Plus only)
// So that supplemental messages are tied together with error/info/warnings
LintMessageGroup Lint::groupLintMessages() noexcept
{
    // Spit out a LintMessageGroup
    LintMessageGroup messageGroup;

    auto firstPtr = m_messages.cbegin();
    auto secondPtr = firstPtr+1;

    // Use two pointers to find groups
    // Point first one to a message
    // Point second to message+1
    // While second pointer points to a supplemental message type, increment and add
    // Point first one to second pointer+1
    // Repeat until end

    while (firstPtr != m_messages.cend() && secondPtr != m_messages.cend())
    {
        // First message type should never be "Supplemental"
        Q_ASSERT(firstPtr->type != Type::TYPE_SUPPLEMENTAL);

        // Add first message
        LintMessages message;
        message.emplace_back(*firstPtr);

        // Associate supplemental messages
        while (secondPtr != m_messages.cend() && secondPtr->type == Type::TYPE_SUPPLEMENTAL)
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

QImage Lint::associateMessageTypeWithIcon(const QString& type) noexcept
{
    Message messageType;

    // Determine type
    if (!QString::compare(type, Type::TYPE_ERROR, Qt::CaseInsensitive))
    {
        messageType = Message::MESSAGE_ERROR;
    }
    else if (!QString::compare(type, Type::TYPE_WARNING, Qt::CaseInsensitive))
    {
        messageType = Message::MESSAGE_WARNING;
    }
    else if (!QString::compare(type, Type::TYPE_INFO, Qt::CaseInsensitive))
    {
        messageType = Message::MESSAGE_INFORMATION;
    }
    else if (!QString::compare(type, Type::TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
    {
        messageType = Message::MESSAGE_SUPPLEMENTAL;
    }
    else
    {
        messageType = Message::MESSAGE_UNKNOWN;
    }

    QImage icon;
    switch (messageType)
    {
        case Message::MESSAGE_ERROR: icon.load(":/images/error.png");  break;
        case Message::MESSAGE_WARNING: icon.load(":/images/warning.png"); break;
        case Message::MESSAGE_INFORMATION:
        case Message::MESSAGE_SUPPLEMENTAL: icon.load(":/images/info.png"); break;
        default: Q_ASSERT(false); break;
    }

    return icon;
}

void Lint::setWorkingDirectory(const QString& directory) noexcept
{
    m_lintDirectory = directory;
}


};

