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

#pragma once

#include <QString>
#include <QMetaType>
#include <QObject>
#include <QSet>
#include <QDebug>
#include <QProcess>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QRegularExpression>
#include <QThread>
#include <QTreeWidget>
#include <QDir>
#include <QFile>
#include <mutex>
#include <memory>
#include <chrono>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include "atomicops.h"
#include "readerwriterqueue.h"

namespace PCLint
{

namespace Xml
{
    // XML tags
    const QString XML_TAG_DOC_OPEN = "<doc>";
    const QString XML_TAG_DOC_CLOSED = "</doc>";
    const QString XML_TAG_MESSAGE_OPEN = "<m>";
    const QString XML_TAG_DESCRIPTION_CLOSED = "</d>";

    // XML elements
    const QString XML_ELEMENT_DOC = "doc";
    const QString XML_ELEMENT_FILE = "f";
    const QString XML_ELEMENT_LINE = "l";
    const QString XML_ELEMENT_MESSAGE_TYPE = "t";
    const QString XML_ELEMENT_MESSAGE_NUMBER = "n";
    const QString XML_ELEMENT_MESSAGE = "m";
    const QString XML_ELEMENT_DESCRIPTION = "d";
};

namespace Type
{
    // PC-Lint Plus types
    const QString TYPE_ERROR = "error";
    const QString TYPE_INFO = "info";
    const QString TYPE_WARNING = "warning";
    const QString TYPE_SUPPLEMENTAL = "supplemental";
};


enum LintStatus
{
    // Lint completed successfully
    STATUS_COMPLETE = 0x0,
    // Lint partially completed but didn't lint all files
    STATUS_PARTIAL_COMPLETE = 0x1,
    // Lint license error
    STATUS_LICENSE_ERROR = 0x4,
    // Lint process error
    STATUS_PROCESS_ERROR = 0x8,
    // Lint process timeout
    STATUS_PROCESS_TIMEOUT = 0x10,
    // Lint cancelled
    STATUS_ABORT = 0x20,
    // Unknown state
    STATUS_UNKNOWN = 0x40
};

enum Message
{
    MESSAGE_ERROR,
    MESSAGE_WARNING,
    MESSAGE_INFORMATION,
    MESSAGE_SUPPLEMENTAL,
    MESSAGE_UNKNOWN,
};

enum Version
{
    VERSION_PC_LINT,
    VERSION_PC_LINT_PLUS,
    VERSION_UNKNOWN
};

constexpr int MAX_PROCESS_CHARACTERS = 8192;
constexpr int MAX_LINT_TIME = 5 * 60 * 1000;
constexpr int MAX_LINT_PATH = 512;
constexpr int MAX_WAIT_TIME = 30 * 1000;
constexpr int MAX_THREAD_WAIT = 30 * 1000;

constexpr int LINT_TABLE_FILE_COLUMN = 0;
constexpr int LINT_TABLE_NUMBER_COLUMN = 1;
constexpr int LINT_TABLE_DESCRIPTION_COLUMN = 2;
constexpr int LINT_TABLE_LINE_COLUMN = 3;

typedef struct
{
    QString file;        // File associated with message
    int line;            // Source code line number
    QString type;        // Message type ("error", "warning", "information", "supplemental")
    int number;          // Message number
    QString description; // Message description
} LintMessage;


using LintMessages = std::vector<LintMessage>;
using LintMessagesSet = QSet<LintMessage>;
using LintMessageGroup = std::vector<LintMessages>;

using namespace moodycamel;

typedef struct
{
    QString linterExecutable;
    QString lintOptionFile;
    QSet<QString> lintFiles;
} LintData;

// Data from a processed lint chunk
typedef struct
{
    LintMessages lintMessages;
    int numberOfErrors;
    int numberOfWarnings;
    int numberOfInformation;
} LintResponse;

class Lint : public QObject
{
    Q_OBJECT
public:
    Lint();
    Lint(const QString& lintExecutable, const QString& lintFile);

    // Set the path to the lint.exe
    void setLintExecutable(const QString& linterExecutable) noexcept;
    // Set the lint file (.lnt) to use
    void setLintFile(const QString& lintFile) noexcept;
    // Set the source files (.c .cpp etc) to lint
    void setSourceFiles(const QSet<QString>& files) noexcept;

    void setLintData(const LintData& lintData) noexcept;



    // Set the working directory for the lint executable
    void setWorkingDirectory(const QString& directory) noexcept;

    // Gets the set of lintMessage returned after a lint
    LintMessages getLintMessages() const noexcept;
    void setLintMessages(const LintMessages& lintMessages) noexcept;

    // Clear all messages and information
    void resetLinter() noexcept;

    int numberOfErrors() const noexcept;
    int numberOfWarnings() const noexcept;
    int numberOfInfo() const noexcept;
    QString errorMessage() const noexcept;

    void setNumberOfErrors(int numberOfErrors) noexcept;
    void setNumberOfWarnings(int numberOfWarnings) noexcept;
    void setNumberOfInfo(int numberOfInfo) noexcept;
    void setErrorMessage(const QString& errorMessage) noexcept;

    void setVersion(const Version& version) noexcept
    {
        m_version = version;
    }


    void toggleMessages(bool toggleErrors, bool toggleWarnings, bool toggleInformation) noexcept;

    // Return path to the lint file used (.lnt)
    QString getLintFile() const noexcept;

    // Group together lint messages (PC-Lint Plus only)
    // So that supplemental messages are tied together with error/info/warnings
    LintMessageGroup groupLintMessages(LintMessages&& lintMessages) noexcept;

    std::vector<QString> parseSourceFileInformation(const QByteArray& data) noexcept;
    LintMessages parseLintMessages(const QByteArray& data);

    std::vector<QByteArray> stitchModule(const QByteArray& data);

public slots:

    void slotAbortLint() noexcept;

    void lint() noexcept;

    void slotPointerToLintTree(QTreeWidget *treeTable) noexcept;

private slots:
    void slotConsumerFinished() noexcept;

signals:
    void signalUpdateProgress(int value);
    void signalUpdateProgressMax(int value);
    void signalUpdateETA(int eta);
    void signalUpdateProcessedFiles(int processedFiles);
    void signalLintProgress(int value);
    void signalLintFinished(const LintResponse& lintResponse);


    void signalLintComplete(const LintStatus& lintStatus, const QString& errorMessage);
    void signalProcessLintMessages(const LintResponse& lintResponse);

    void signalProcessLintMessageGroup(const LintMessageGroup& lintMessageGroup);


    void signalUpdateErrors();
    void signalUpdateWarnings();
    void signalUpdateInformations();

    void signalConsumerFinished();

private:
    QString m_lintDirectory;
    QStringList m_arguments;
    QString m_lintExecutable;
    QString m_lintFile;
    QSet<QString> m_filesToLint;
    LintMessages m_messages;
    int m_numberOfErrors;
    int m_numberOfWarnings;
    int m_numberOfInfo;
    QString m_errorMessage;
    Version m_version;

    // stderr has the module (file lint) progress
    // stdout has the actual data

    LintStatus m_status;
    int m_numberOfLintedFiles;

    QFile m_stdOutFile;
    QFile m_stdErrFile;


    QByteArray m_stdOut;

    std::unique_ptr<QProcess> m_process;


    void emitLintComplete() noexcept;
    void consumeLintChunk() noexcept;


    void addTreeMessageGroup(const PCLint::LintMessageGroup& lintMessageGroup) noexcept;
    bool filterMessageType(const QString& type) const noexcept;

    void processModules(std::vector<QByteArray> modules);

    LintMessagesSet m_messageSet;

    bool m_finished;

    std::unique_ptr<ReaderWriterQueue<QByteArray>> m_dataQueue;
    std::mutex m_consumerMutex;
    std::condition_variable m_consumerConditionVariable;
    std::unique_ptr<std::thread> m_consumerThread;

    QTreeWidget* m_treeTable;

    bool m_toggleError;
    bool m_toggleWarning;
    bool m_toggleInformation;

    QFile m_testFile;
};

// For QSet
inline bool operator==(const LintMessage &e1, const LintMessage &e2) noexcept
{
    return (e1.number == e2.number) &&
            (e1.file == e2.file) &&
            (e1.line == e2.line) &&
            (e1.type == e2.type) &&
            (e1.description == e2.description);
}

inline uint qHash(const LintMessage& key, uint seed) noexcept
{
    return qHash(key.file + key.type + key.description, seed) ^ key.line ^ key.number;
}

};
