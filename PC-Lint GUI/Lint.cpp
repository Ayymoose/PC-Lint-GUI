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
    if (m_future.isRunning())
    {
        m_finished = true;
        m_conditionVariable.notify_one();
        m_future.waitForFinished();
    }
    m_process->closeReadChannel(QProcess::StandardOutput);
    m_process->closeReadChannel(QProcess::StandardError);
    m_process->close();
}

void Lint::setLintFile(const QString& lintFile) noexcept
{
    m_lintFile = lintFile;
}

void Lint::setLintExecutable(const QString& linterExecutable) noexcept
{
    m_lintExecutable = linterExecutable;
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
    //m_arguments << ("-unit_check");

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

    // Add the lint file
    m_arguments << (m_lintFile);

    for (const auto& str : m_arguments)
    {
        cmdString += " \"" + str + "\"";
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


    m_stdErrFile.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\stderr.xml)");
    m_stdErrFile.remove();
    if (!m_stdErrFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        Q_ASSERT(false);
    }

    m_stdOutFile.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\stdout.xml)");
    m_stdOutFile.remove();
    if (!m_stdOutFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        Q_ASSERT(false);
    }

    m_lintStdOut.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\lintout.xml)");
    m_lintStdOut.remove();
    if (!m_lintStdOut.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        Q_ASSERT(false);
    }

    // New data queue
    m_dataQueue = std::make_unique<ReaderWriterQueue<QByteArray>>();

    // Start consumer thread here
    m_future = QtConcurrent::run(this, &Lint::consumeLintChunk);
            //std::make_unique<std::thread>(&Lint::consumeLintChunk, this);

    m_process->setProgram(m_lintExecutable);
    m_process->setArguments(m_arguments);
    m_process->start();

    // No local variables beyond this point

    QObject::connect(m_process.get(), &QProcess::started, this, [this]()
    {
        //lintStartTime = std::chrono::steady_clock::now();
        // Tell ProgressWindow the maximum number of files we have
        //emit signalUpdateProgressMax(m_filesToLint.size());
        //emit signalUpdateProcessedFiles(m_numberOfLintedFiles);
    });

    QObject::connect(m_process.get(), static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
    [this](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "Lint process finished with exit code:" << QString::number(exitCode) << "and exit status:" << exitStatus;
        slotAbortLint();

        if (m_status & (STATUS_PROCESS_ERROR | STATUS_PROCESS_TIMEOUT | STATUS_ABORT | STATUS_LICENSE_ERROR))
        {
            Q_ASSERT(!m_errorMessage.isEmpty());
            emit signalLintComplete(m_status, m_errorMessage);
            return;
        }


        // TODO: Fix successful lint
        m_status = STATUS_COMPLETE;
        // if -env_push is used we'll lint "more" files than we have since it does multiple passes
        /*if (m_numberOfLintedFiles >= m_filesToLint.size())
        {
            qDebug() << "Successfully linted all files";
            m_status = STATUS_COMPLETE;
        }
        else
        {
            qDebug() << "Linted only" << m_numberOfLintedFiles << '/' << m_filesToLint.size() << "files";
            m_status = STATUS_PARTIAL_COMPLETE;
        }*/

        emit signalLintComplete(m_status, m_errorMessage);

    });

    QObject::connect(m_process.get(), &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        if (m_status != STATUS_ABORT)
        {
            qWarning() << __FUNCTION__ << "error occurred:" << error;
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
            m_lintStdOut.write(readStdOut);

            // Lock free queue needed here
            // This section must never block otherwise the GUI will hang
            m_dataQueue->enqueue(std::move(readStdOut));
            m_conditionVariable.notify_one();
        }
        catch (const std::exception& e)
        {
            qCritical() << __FUNCTION__ << "Exception caught:" << e.what();
            m_errorMessage = e.what();
            slotAbortLint();
        }
        catch (...)
        {
            qCritical() << __FUNCTION__ << "unknown exception caught";
            m_errorMessage = "Unknown exception caught when processing data";
            slotAbortLint();
        }

    });

    QObject::connect(m_process.get(), &QProcess::readyReadStandardError, this,[this]()
    {
        auto stdErrData = m_process->readAllStandardError();

        m_stdErrFile.write(stdErrData);

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

        auto const sourceFiles = parseSourceFileInformation(stdErrData);
        for (auto const& sourceFile : sourceFiles)
        {
            qInfo() << "Linted:" << sourceFile;
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

    for (;;)
    {
        // TODO: Is it possible to deadlock here?
        std::unique_lock<std::mutex> lock(m_mutex);
        m_conditionVariable.wait(lock,[this]{ return (m_dataQueue->size_approx() != 0 || m_finished);});

        if (m_finished)
        {
            break;
        }

        QByteArray lintChunk;
        bool success = m_dataQueue->try_dequeue(lintChunk);
        if (!success)
        {
            continue;
        }

        // PC-Lint Plus process will spit out chunks of data, not complete module processed output but parts
        // We must piece them together by extracting data between pairs of "--- Module:   " tags

        try
        {
            // Process module data
            auto modules = stitchModule(lintChunk);
            processModules(modules);
        }
        catch (const std::exception& e)
        {
            qCritical() << __FUNCTION__ << '(' << __LINE__ << ')' << "Exception caught:" << e.what();
            m_errorMessage = "Exception caught when processing module: ";
            m_errorMessage += e.what();
            m_status = STATUS_PROCESS_ERROR;
            return;
        }

    }

    qInfo() << __FUNCTION__ << "processing last remaining data";

    // Producer processed finished before consumer did
    // Deqeue all items and process them
    for (;;)
    {
        try
        {
            QByteArray lintChunk;
            m_dataQueue->try_dequeue(lintChunk);

            // Process a single chunk of data
            auto modules = stitchModule(lintChunk);

            // No more data left to process, exit
            if (modules.size() == 0)
            {

                if (!m_stdOut.contains("</doc>"))
                {
                    m_stdOut.append("</doc>");
                }
                else
                {
                    // Debug only
                    m_stdOutFile.write(m_stdOut);
                    break;
                }
            }

            processModules(modules);
        }
        catch (const std::exception& e)
        {
            qCritical() << __FUNCTION__ << '(' << __LINE__ << ')' << "Exception caught:" << e.what();
            m_errorMessage = "Exception caught when processing module: ";
            m_errorMessage += e.what();
            m_status = STATUS_PROCESS_ERROR;
            return;
        }
    }
}

LintMessages Lint::parseLintMessages(const QByteArray& data)
{
    // Ordering of messages is now important (was QSet)
    QXmlStreamReader lintXML(data);
    LintMessages lintMessages;
    LintMessage message;

    // Start XML parsing
    // Parse the XML until we reach end of it

    while (!lintXML.atEnd() && !lintXML.hasError())
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
            }
            else if (lintXML.name() == Xml::XML_ELEMENT_MESSAGE_NUMBER)
            {
                // <n> tag
                message.number = lintXML.readElementText().toInt();
            }
            else if (lintXML.name() == Xml::XML_ELEMENT_DESCRIPTION)
            {
                // <d> tag
                message.description = lintXML.readElementText();
            }

        }


        if ((lintXML.name() == Xml::XML_ELEMENT_MESSAGE) && (token == QXmlStreamReader::EndElement))
        {
            // Lint can spit out duplicate messages for different files
            // So we must remove them otherwise we'd consume a huge chunk of memory

            if (m_messageSet.find(message) == m_messageSet.end())
            {
                // Don't add supplementals first
                if (!((lintMessages.size() == 0) && (message.type == Type::TYPE_SUPPLEMENTAL)))
                {
                    m_messageSet.insert(message);
                    lintMessages.emplace_back(std::move(message));
                }
            }
        }
    }

    if (lintXML.hasError())
    {
        qCritical() << "XML parser error";
        qCritical() << "Error String:" << lintXML.errorString();
        qCritical() << "Line Number:" << QString::number(lintXML.lineNumber());
        qCritical() << "Column Number:" << QString::number(lintXML.columnNumber());
        qCritical() << "Character Offset:" << QString::number(lintXML.characterOffset());
        m_stdOutFile.write(data);
        m_stdOutFile.flush();
        throw std::runtime_error("XML parser error occurred");
    }

    return lintMessages;
}

void Lint::processModules(std::vector<QByteArray> modules)
{
    for (auto const& module : modules)
    {
         // Grab lint messages from data
         LintMessages lintMessages = parseLintMessages(module);

         // Group lint messages together
         auto groupedLintMessages = groupLintMessages(std::move(lintMessages));

         // Send chunk of data to be processed
         emit signalAddTreeMessageGroup(groupedLintMessages);
    }
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

    while (firstPtr != lintMessages.cend())
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

void Lint::setWorkingDirectory(const QString& directory) noexcept
{
    m_lintDirectory = directory;
}

// Parse the byte array of data for the source files in the PC-Lint Plus output
std::vector<QString> Lint::parseSourceFileInformation(const QByteArray& data) noexcept
{
    std::vector<QString> sourceFiles;

    size_t from = 0;
    for (;;)
    {
        // TODO: Might be easier/more efficent to use split on "--- Module:   "
        int firstIndex = data.indexOf("--- Module:   ", from);
        if (firstIndex == -1)
        {
            break;
        }
        int secondIndex = data.indexOf("\r\n", firstIndex+1);
        if (secondIndex == -1)
        {
            break;
        }
        auto sourceFile = data.mid(firstIndex + QString("--- Module:   ").length(),
                                         secondIndex-firstIndex-QString("--- Module:   ").length());


        if (sourceFile.endsWith(" (C++)"))
        {
            sourceFile.chop(QString(" (C++)").length());
        }
        else if (sourceFile.endsWith(" (C)"))
        {
            sourceFile.chop(QString(" (C)").length());
        }
        else
        {
            Q_ASSERT(false);
        }

        sourceFiles.emplace_back(sourceFile);
        from = secondIndex;
    }

    return sourceFiles;
}

// Process a chunk of data if possible?
std::vector<QByteArray> Lint::stitchModule(const QByteArray& data)
{
    std::vector<QByteArray> modules;

    m_stdOut.append(data);

    for (;;)
    {
        int firstIndex = m_stdOut.indexOf("--- Module:   ");
        if (firstIndex == -1)
        {
            return modules;
        }
        int secondIndex = m_stdOut.indexOf("--- Module:   ", firstIndex+1);
        if (secondIndex == -1)
        {
            // It could be the end of the document (ends with "</doc>"
            secondIndex = m_stdOut.indexOf(Xml::XML_TAG_DOC_CLOSED, firstIndex+1);
            if (secondIndex == -1)
            {
                return modules;
            }
        }

        // We have a complete module of data, chop and process
        auto module = m_stdOut.mid(firstIndex, secondIndex-firstIndex);

        // Remove this from our array to lower memory usage
        m_stdOut.remove(firstIndex, secondIndex-firstIndex);

        // Each module chunk will need a pair of <doc>/<doc> tags
        // wrapped around them for the XML stream reader to work


        module.replace("<doc>", "");
        module.replace("</doc>", "");

        module.prepend("<doc>");
        module.append("</doc>");

        // Add this to the array
        modules.emplace_back(module);
    }

}

};

