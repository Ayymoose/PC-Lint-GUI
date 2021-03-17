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



// TODO: Make namespace Lint and this class PCLintPlus
namespace PCLint
{

Lint::Lint() :
    m_numberOfErrors(0),
    m_numberOfWarnings(0),
    m_numberOfInfo(0),
    m_version(VERSION_UNKNOWN),
    m_status(STATUS_UNKNOWN),
    m_numberOfLintedFiles(0),
    m_finished(false)
{

}

Lint::Lint(const QString& lintExecutable, const QString& lintFile) :
    m_lintExecutable(lintExecutable),
    m_lintFile(lintFile),
    m_numberOfErrors(0),
    m_numberOfWarnings(0),
    m_numberOfInfo(0),
    m_version(VERSION_UNKNOWN),
    m_status(STATUS_UNKNOWN),
    m_numberOfLintedFiles(0),
    m_finished(false)
{

}

void Lint::slotAbortLint() noexcept
{
    if (m_consumerThread->joinable())
    {
        m_finished = true;
        m_consumerConditionVariable.notify_one();
        m_consumerThread->join();
    }
    m_process->closeReadChannel(QProcess::StandardOutput);
    m_process->closeReadChannel(QProcess::StandardError);
    m_process->close();
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

void Lint::lint() noexcept
{
    Q_ASSERT(m_lintFile.size());
    Q_ASSERT(m_lintExecutable.size());

    auto const workingDirectory = QFileInfo(m_lintFile).canonicalPath();
    qDebug() << "Setting working directory to: " << workingDirectory;

    m_process = std::make_unique<QProcess>();

    m_process->setWorkingDirectory(workingDirectory);

    // stderr has the module (file lint) progress
    // sttout has the actual data
    QString cmdString = m_lintExecutable;

    // Reset
    m_arguments.clear();
    m_status = STATUS_UNKNOWN;
    m_numberOfErrors = 0;
    m_numberOfInfo = 0;
    m_numberOfWarnings = 0;
    m_numberOfLintedFiles = 0;
    m_finished = false;
    m_messageSet.clear();
    m_stdOut.clear();

    // TODO: Put in function to process and parse lint arguments
    m_arguments << ("-max_threads=8");

    // For testing, does 1 pass only
    m_arguments << ("-unit_check");

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

    qInfo() << "Lint path: " << m_lintExecutable;
    qInfo() << "Lint file: " << m_lintFile;

    // Temporary debug information
    QFile commandFileDebug(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\cmdline.xml)");
    commandFileDebug.open(QIODevice::WriteOnly);
    Q_ASSERT(commandFileDebug.isOpen());
    auto bytesWritten = commandFileDebug.write(cmdString.toLocal8Bit());
    Q_ASSERT(bytesWritten > 0);
    commandFileDebug.close();


    m_lintFailedFile.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\failedLintChunk.xml)");
    if (!m_lintFailedFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        Q_ASSERT(false);
    }

    m_lintOutFile.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\lintChunks.xml)");
    if (!m_lintOutFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        Q_ASSERT(false);
    }

    // New data queue
    m_dataQueue = std::make_unique<ReaderWriterQueue<QByteArray>>();

    // Start consumer thread here
    m_consumerThread = std::make_unique<std::thread>(&Lint::consumeLintChunk, this);

    m_process->setProgram(m_lintExecutable);
    m_process->setArguments(m_arguments);
    m_process->start();

    // No local variables beyond this point

    QObject::connect(m_process.get(), &QProcess::started, this, [this]()
    {
        //lintStartTime = std::chrono::steady_clock::now();
        // Tell ProgressWindow the maximum number of files we have
        emit signalUpdateProgressMax(m_filesToLint.size());
        //emit signalUpdateProcessedFiles(m_numberOfLintedFiles);
    });

    QObject::connect(m_process.get(), static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
    [this](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "Lint process finished with exit code:" << QString::number(exitCode) << "and exit status:" << exitStatus;
        slotAbortLint();
        if (m_status & (STATUS_PROCESS_ERROR | STATUS_PROCESS_TIMEOUT | STATUS_ABORT | STATUS_LICENSE_ERROR))
        {
            emit signalLintComplete(m_status, m_errorMessage);
            return;
        }

        // if -env_push is used we'll lint "more" files than we have since it does multiple passes
        if (m_numberOfLintedFiles >= m_filesToLint.size())
        {
            qDebug() << "Successfully linted all files";
            m_status = STATUS_COMPLETE;
        }
        else
        {
            qDebug() << "Linted only" << m_numberOfLintedFiles << '/' << m_filesToLint.size() << "files";
            m_status = STATUS_PARTIAL_COMPLETE;
        }

        emit signalLintComplete(m_status, m_errorMessage);

    });

    // TODO: Use multiple threads if the user specifies if we are linting a single project (PC-Lint Plus)
    // Or need some kind of balancing algorithm?

    QObject::connect(m_process.get(), &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        if (m_status != STATUS_ABORT)
        {
            qWarning() << __FUNCTION__ << " error occurred: " << error;
            m_status = STATUS_PROCESS_ERROR;
        }
    });

    QObject::connect(m_process.get(), &QProcess::readyReadStandardOutput, this, [this]()
    {
        // TODO: Set timer here to expire if lint per file takes too long
        try
        {
            // On large projects, there's a good chance we'll get a bad_alloc thrown
            auto readStdOut = m_process->readAllStandardOutput();

            // Lock free queue needed
            // This section must never block otherwise the GUI will hang
            m_dataQueue->enqueue(std::move(readStdOut));
            m_consumerConditionVariable.notify_one();
        }
        catch (const std::exception& e)
        {
            qWarning() << __FUNCTION__ << e.what();
            slotAbortLint();
        }
        catch (...)
        {
            qWarning() << "Caught unknown exception";
            slotAbortLint();
        }

    });

    QObject::connect(m_process.get(), &QProcess::readyReadStandardError, this,[this]()
    {
        auto stdErrData = m_process->readAllStandardError();

        // Check if license is valid
        // PC-Lint Plus version is always the first line included in stderr
        if (stdErrData.contains("License Error"))
        {
            m_status = STATUS_LICENSE_ERROR;

            qCritical() << "Lint failed with license error:\n" << stdErrData;
            m_errorMessage = stdErrData;

            slotAbortLint();
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

void Lint::consumeLintChunk() noexcept
{
    // While the queue isn't empty or we haven't finished, dequeues items for processing
    // Producer will enqueue items while there is data

    while (!m_finished)
    {
        std::unique_lock<std::mutex> lock(m_consumerMutex);
        m_consumerConditionVariable.wait(lock,[this]{ return (m_dataQueue->size_approx() != 0 || m_finished);});

        QByteArray lintChunk;
        bool success = m_dataQueue->try_dequeue(lintChunk);
        if (!success)
        {
            continue;
        }

        m_stdOut.append(lintChunk);

        int firstIndex = m_stdOut.indexOf("--- Module:   ");
        if (firstIndex == -1)
        {
            continue;
        }
        int secondIndex = m_stdOut.indexOf("--- Module:   ", firstIndex+1);
        if (secondIndex == -1)
        {
            continue;
        }

        // We have a complete module of data, chop and process
        auto moduleData = m_stdOut.mid(firstIndex, secondIndex-firstIndex);

        // Remove this from our array
        m_stdOut.replace(firstIndex, secondIndex-firstIndex, "");



        // Now we can process them
        int numberOfErrors = 0;
        int numberOfWarnings = 0;
        int numberOfInformation = 0;

        // Fix XML doc tags that are littered about
        moduleData.replace(Xml::XML_TAG_DOC_OPEN,"");
        moduleData.replace(Xml::XML_TAG_DOC_CLOSED,"");
        moduleData.prepend("<doc>");
        moduleData.append(Xml::XML_TAG_DOC_CLOSED);


        // Ordering of messages is now important (was QSet)
        LintMessages lintMessages;
        QXmlStreamReader lintXML(moduleData);
        LintMessage message;
        m_lintOutFile.write(moduleData);

        // Start XML parsing
        // Parse the XML until we reach end of it
        try
        {
            while(!lintXML.atEnd() && !lintXML.hasError())
            {
                // Read next element
                auto const token = lintXML.readNext();
                //If token is just StartDocument - go to next
                if (token == QXmlStreamReader::StartDocument)
                {
                    continue;
                }

                //If token is StartElement - read it
                if (token == QXmlStreamReader::StartElement)
                {
                    // <doc> or <m> tag
                    if (lintXML.name() == Xml::XML_ELEMENT_DOC || lintXML.name() == Xml::XML_ELEMENT_MESSAGE)
                    {
                        continue;
                    }

                    if (lintXML.name() == Xml::XML_ELEMENT_FILE)
                    {
                        // <f> tag
                        message.file = lintXML.readElementText();
                        // Why does PC-Lint Plus mess with the directory separator?
                        // It spits out '/' and '\' in the same path which messages with the hash for QSet
                        // TODO: I still see some duplicate output in the tree; investigate
                        message.file = QDir::toNativeSeparators(message.file);
                    }
                    else if (lintXML.name() == Xml::XML_ELEMENT_LINE)
                    {
                        // <l> tag
                        message.line = lintXML.readElementText().toInt();
                    }
                    else if (lintXML.name() == Xml::XML_ELEMENT_MESSAGE_TYPE)
                    {
                        // <t> tag
                        message.type = lintXML.readElementText();
                        Q_ASSERT(message.type.length() > 0);
                    }
                    else if (lintXML.name() == Xml::XML_ELEMENT_MESSAGE_NUMBER)
                    {
                        // <n> tag
                        message.number = lintXML.readElementText().toInt();
                        Q_ASSERT(message.number > 0);
                    }
                    else if (lintXML.name() == Xml::XML_ELEMENT_DESCRIPTION)
                    {
                        // <d> tag
                        message.description = lintXML.readElementText();
                        Q_ASSERT(message.description.length() > 0);
                    }

                }


                if ((token == QXmlStreamReader::EndElement) && (lintXML.name() == Xml::XML_ELEMENT_MESSAGE))
                {
                    // Lint can spit out duplicate messages for different files
                    // So we must remove them otherwise we'd consume a huge chunk of memory

                    if (m_messageSet.find(message) == m_messageSet.end())
                    {
                        // Ascertain type
                        if (message.type == Type::TYPE_ERROR)
                        {
                            numberOfErrors++;
                        }
                        else if (message.type == Type::TYPE_WARNING)
                        {
                            numberOfWarnings++;
                        }
                        else if (message.type == Type::TYPE_INFO)
                        {
                            numberOfInformation++;
                        }
                        else if (message.type == Type::TYPE_SUPPLEMENTAL)
                        {
                            // Don't care about number of supplementals
                        }
                        else
                        {
                            Q_ASSERT(false);
                        }

                        // Don't add supplementals first
                        if (!((lintMessages.size() == 0) && (message.type == Type::TYPE_SUPPLEMENTAL)))
                        {
                            m_messageSet.insert(message);
                            lintMessages.emplace_back(std::move(message));
                        }

                    }


                }
            }

        }
        catch (const std::exception& e)
        {
            // TODO: Test
            qCritical() << __FUNCTION__ << e.what();
            m_status = STATUS_PROCESS_ERROR;
            return;
        }

        if (lintXML.hasError())
        {
            // TODO: Test
            qCritical() << "XML parser error";
            qCritical() << "Error String:" << lintXML.errorString();
            qCritical() << "Line Number:" << QString::number(lintXML.lineNumber());
            qCritical() << "Column Number:" << QString::number(lintXML.columnNumber());
            qCritical() << "Character Offset:" << QString::number(lintXML.characterOffset());
            m_status = STATUS_PROCESS_ERROR;
            return;
        }

        // Group lint messages together
        auto groupedLintMessages = groupLintMessages(std::move(lintMessages));

        // Send chunk of data to be processed
        // TODO: Send number of errors warnings etc
        emit signalProcessLintMessageGroup(groupedLintMessages);

       /* emit signalProcessLintMessages(LintResponse
        {
           ,
           numberOfErrors,
           numberOfWarnings,
           numberOfInformation
        });*/


    }

}


LintResponse Lint::testLintProcessMessages(QByteArray& lintChunk) noexcept
{
    int numberOfErrors = 0;
    int numberOfWarnings = 0;
    int numberOfInformation = 0;

    // Fix XML doc tags that are littered about
    lintChunk.replace(Xml::XML_TAG_DOC_OPEN,"");
    lintChunk.replace(Xml::XML_TAG_DOC_CLOSED,"");
    lintChunk.prepend("<doc>");
    lintChunk.append(Xml::XML_TAG_DOC_CLOSED);


    // Ordering of messages is now important (was QSet)
    LintMessages lintMessages;

    QXmlStreamReader lintXML(lintChunk);
    LintMessage message;

    int messagesAdded = 0;
    auto startLintTime = std::chrono::steady_clock::now();

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
                // It spits out '/' and '\' in the same path which messages with the hash for QSet
                // TODO: I still see some duplicate output in the tree; investigate
                message.file = QDir::toNativeSeparators(message.file);
            }

            // <l> tag
            if(lintXML.name() == Xml::XML_ELEMENT_LINE)
            {
                message.line = lintXML.readElementText().toInt();
                Q_ASSERT(message.line > 0);
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
                message.number = lintXML.readElementText().toInt();
                Q_ASSERT(message.number > 0);
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
            // Lint can spit out duplicate messages for different files
            // So we must remove them otherwise we'd consume a huge chunk of memory
            if (m_messageSet.find(message) == m_messageSet.end())
            {
                messagesAdded++;
                // Ascertain type
                if (!QString::compare(message.type, Type::TYPE_ERROR, Qt::CaseInsensitive))
                {
                    numberOfErrors++;
                }
                else if (!QString::compare(message.type, Type::TYPE_WARNING, Qt::CaseInsensitive))
                {
                    numberOfWarnings++;
                }
                else if (!QString::compare(message.type, Type::TYPE_INFO, Qt::CaseInsensitive))
                {
                    numberOfInformation++;
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

                // Don't add supplementals first
                if (!(lintMessages.size() == 0 && !QString::compare(message.type, Type::TYPE_SUPPLEMENTAL, Qt::CaseInsensitive)))
                {
                    m_messageSet.insert(message);
                    lintMessages.emplace_back(std::move(message));
                }

            }

            // TODO: This probably isn't needed
            message = {};
        }


    }

    auto endLintTime = std::chrono::steady_clock::now();
    auto totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    if (messagesAdded)
    {
        qDebug() << "Messages added:" << QString::number(messagesAdded) <<  "processLintMessagesXML" << totalElapsedTime / 1000.f << "s elapsed";
    }

    if (lintXML.hasError())
    {
        qCritical() << "XML parser error";
        qCritical() << "Error Type:" << QString(lintXML.error());
        qCritical() << "Error String:" << lintXML.errorString();
        qCritical() << "Line Number:" << QString::number(lintXML.lineNumber());
        qCritical() << "Column Number:" << QString::number(lintXML.columnNumber());
        qCritical() << "Character Offset:" << QString::number(lintXML.characterOffset());
        // TODO: Return process error
        std::terminate();
    }

    // Send chunk of data to be processed
    return LintResponse
    {
       std::move(lintMessages),
       numberOfErrors,
       numberOfWarnings,
       numberOfInformation
    };
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

LintMessageGroup Lint::groupLintMessages(LintMessages&& lintMessages) noexcept
{
    // Spit out a LintMessageGroup
    LintMessageGroup messageGroup;

    auto firstPtr = lintMessages.cbegin();
    auto secondPtr = firstPtr+1;

    // Use two pointers to find groups
    // Point first one to a message
    // Point second to message+1
    // While second pointer points to a supplemental message type, increment and add
    // Point first one to second pointer+1
    // Repeat until end

    while (firstPtr != lintMessages.cend() && secondPtr != lintMessages.cend())
    {
        // First message type should never be "Supplemental"
        Q_ASSERT(firstPtr->type != Type::TYPE_SUPPLEMENTAL);

        // Add first message
        LintMessages message;
        message.emplace_back(*firstPtr);

        // Associate supplemental messages
        while (secondPtr != lintMessages.cend() && secondPtr->type == Type::TYPE_SUPPLEMENTAL)
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
    QImage icon;

    // TODO: Re-order with thich one is more likely
    if (type == Type::TYPE_ERROR)
    {
        icon.load(":/images/error.png");
    }
    else if (type == Type::TYPE_WARNING)
    {
        icon.load(":/images/warning.png");
    }
    else if (type == Type::TYPE_INFO)
    {
        icon.load(":/images/info.png");
    }
    else if (type == Type::TYPE_SUPPLEMENTAL)
    {
        icon.load(":/images/info.png");
    }
    else
    {
        Q_ASSERT(false);
    }

    return icon;
}

void Lint::setWorkingDirectory(const QString& directory) noexcept
{
    m_lintDirectory = directory;
}


};

