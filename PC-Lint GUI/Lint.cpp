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
    m_finished(false),
    m_treeTable(nullptr),
    m_toggleError(false),
    m_toggleWarning(false),
    m_toggleInformation(false)
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
    m_finished(false),
    m_treeTable(nullptr),
    m_toggleError(false),
    m_toggleWarning(false),
    m_toggleInformation(false)
{

}

// If this message type should be filtered then returns true, false otherwise
bool Lint::filterMessageType(const QString& type) const noexcept
{
    bool filter = false;
    if (!m_toggleError && (type == Type::TYPE_ERROR))
    {
        filter = true;
    }
    else if (!m_toggleWarning && (type == Type::TYPE_WARNING))
    {
        filter = true;
    }
    else if (!m_toggleInformation && (type == Type::TYPE_INFO))
    {
        filter = true;
    }
    return filter;
}


void Lint::addTreeMessageGroup(const PCLint::LintMessageGroup& lintMessageGroup) noexcept
{
    Q_ASSERT(m_treeTable);

    // Create a new node in the lint tree table
    auto createTreeNode = [this](QTreeWidgetItem* parentItem, const PCLint::LintMessage& message)
    {
        auto* treeItem = new QTreeWidgetItem(parentItem);
        treeItem->setText(LINT_TABLE_FILE_COLUMN, QFileInfo(message.file).fileName());
        treeItem->setData(LINT_TABLE_FILE_COLUMN, Qt::UserRole, message.file);
        treeItem->setText(LINT_TABLE_NUMBER_COLUMN, QString::number(message.number));
        treeItem->setText(LINT_TABLE_DESCRIPTION_COLUMN, message.description);
        treeItem->setText(LINT_TABLE_LINE_COLUMN, QString::number(message.line));

        QImage icon;

        if (message.type == Type::TYPE_ERROR)
        {
            icon.load(":/images/error.png");
            emit signalUpdateErrors();
        }
        else if (message.type == Type::TYPE_WARNING)
        {
            icon.load(":/images/warning.png");
            emit signalUpdateWarnings();
        }
        else if (message.type == Type::TYPE_INFO)
        {
            icon.load(":/images/info.png");
            emit signalUpdateInformations();
        }
        else if (message.type == Type::TYPE_SUPPLEMENTAL)
        {
            icon.load(":/images/info.png");
        }
        else
        {
            Q_ASSERT(false);
        }

        treeItem->setData(LINT_TABLE_FILE_COLUMN, Qt::DecorationRole, QPixmap::fromImage(icon));
        return treeItem;
    };

    for (const auto& groupMessage : lintMessageGroup)
    {
        // Every vector must be at least 1 otherwise something went wrong
        Q_ASSERT(groupMessage.size() >= 1);

        // Create the top-level item
        const auto messageTop = groupMessage.front();

        // Group together items under the same file
        auto const messageTopFileName = QFileInfo(messageTop.file).fileName();
        auto const treeList = m_treeTable->findItems(messageTopFileName,Qt::MatchExactly, LINT_TABLE_FILE_COLUMN);

        QTreeWidgetItem* fileDetailsItemTop;

        if (treeList.size())
        {
            // Should only ever be 1 file name, unless there are mutiple files with the same name but different path?
            // Already exists, use this one
            Q_ASSERT(treeList.size() == 1);
            fileDetailsItemTop = treeList.first();
        }
        else
        {
            // New top level file entry
            fileDetailsItemTop = new QTreeWidgetItem(m_treeTable);
            fileDetailsItemTop->setText(LINT_TABLE_FILE_COLUMN, messageTopFileName);
            fileDetailsItemTop->setData(LINT_TABLE_FILE_COLUMN, Qt::UserRole, messageTop.file);
        }

        // Filter
        if (filterMessageType(messageTop.type))
        {
            continue;
        }

        // If it's just a single entry
        if (groupMessage.size() == 1)
        {
            createTreeNode(fileDetailsItemTop, messageTop);
            // Skip next
            continue;
        }

        // Create a child level item
        auto fileDetailsItem = createTreeNode(fileDetailsItemTop, messageTop);

        Q_ASSERT(groupMessage.size() > 1);

        // Otherwise grab the rest of the group
        for (auto cit = groupMessage.cbegin()+1; cit != groupMessage.cend(); cit++)
        {
            auto message = *cit;

            // Filter
            // TODO: Should this be message.type?!
            if (filterMessageType(messageTop.type))
            {
                continue;
            }

            // Check if the file exists (absolute path given)
            // Check if it exists in the project file's directory
            if (!QFile(message.file).exists())
            {
                // TODO: Optimise this
                const auto relativeFile = QFileInfo(m_lintFile).canonicalPath() + '/' + message.file;
                if (!QFile(relativeFile).exists())
                {
                    message.file = "";
                }
                else
                {
                    message.file = relativeFile;
                }
            }
            // The sticky details
            createTreeNode(fileDetailsItem, message);
        }
    }

}

void Lint::slotPointerToLintTree(QTreeWidget* treeTable) noexcept
{
    m_treeTable = treeTable;
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

void Lint::toggleMessages(bool toggleErrors, bool toggleWarnings, bool toggleInformation) noexcept
{
    m_toggleError = toggleErrors;
    m_toggleWarning = toggleWarnings;
    m_toggleInformation = toggleInformation;
}

void Lint::lint() noexcept
{
    Q_ASSERT(m_lintFile.size());
    Q_ASSERT(m_lintExecutable.size());
    Q_ASSERT(m_treeTable);

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
    m_messages.clear();
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

    m_stdOutFile.remove();
    m_stdOutFile.setFileName(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\stdout.xml)");
    if (!m_stdOutFile.open(QIODevice::WriteOnly | QIODevice::Append))
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
        std::unique_lock<std::mutex> lock(m_consumerMutex);
        m_consumerConditionVariable.wait(lock,[this]{ return (m_dataQueue->size_approx() != 0 || m_finished);});

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
            qCritical() << e.what();
            m_status = STATUS_PROCESS_ERROR;
            return;
        }

    }

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
                m_stdOutFile.write(m_stdOut);
                break;
            }

            processModules(modules);

        }
        catch (const std::exception& e)
        {
            qCritical() << e.what();
            m_status = STATUS_PROCESS_ERROR;
            return;
        }
    }

    emit signalConsumerFinished();
}

void Lint::slotConsumerFinished() noexcept
{

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
        // TODO: Test
        qCritical() << "XML parser error";
        qCritical() << "Error String:" << lintXML.errorString();
        qCritical() << "Line Number:" << QString::number(lintXML.lineNumber());
        qCritical() << "Column Number:" << QString::number(lintXML.columnNumber());
        qCritical() << "Character Offset:" << QString::number(lintXML.characterOffset());
        m_stdOutFile.write(data);
        m_stdOutFile.flush();
        throw;
    }

    return lintMessages;
}

void Lint::processModules(std::vector<QByteArray> modules)
{
    for (auto const& module : modules)
    {
         // Grab lint messages from data
         LintMessages lintMessages = parseLintMessages(module);

         // Append messages together, we must keep track of this for the lint table message filtering
         m_messages.insert(m_messages.end(),lintMessages.begin(), lintMessages.end());

         // Group lint messages together
         auto groupedLintMessages = groupLintMessages(std::move(lintMessages));

         // Send chunk of data to be processed
         addTreeMessageGroup(groupedLintMessages);
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
        // Might be easier to use split
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

        Q_ASSERT(!module.contains("<doc>"));
        Q_ASSERT(!module.contains("</doc>"));

        module.prepend("<doc>");
        module.append("</doc>");

        // Add this to the array
        modules.emplace_back(module);
    }

}

};

