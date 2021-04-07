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
#include <QFuture>
#include <QStandardItemModel>
#include <QtConcurrent>
#include <mutex>
#include <memory>
#include <chrono>
#include <vector>
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include "atomicops.h"
#include "readerwriterqueue.h"

namespace Lint
{

namespace Xml
{
    // XML tags
    constexpr const char* XML_TAG_DOC_OPEN = "<doc>";
    constexpr const char* XML_TAG_DOC_CLOSED = "</doc>";

    // XML elements
    constexpr const char* XML_ELEMENT_DOC = "doc";
    constexpr const char* XML_ELEMENT_FILE = "f";
    constexpr const char* XML_ELEMENT_LINE = "l";
    constexpr const char* XML_ELEMENT_MESSAGE_TYPE = "t";
    constexpr const char* XML_ELEMENT_MESSAGE_NUMBER = "n";
    constexpr const char* XML_ELEMENT_MESSAGE = "m";
    constexpr const char* XML_ELEMENT_DESCRIPTION = "d";
};

namespace Type
{
    // PC-Lint Plus types
    const QString TYPE_ERROR = "error";
    const QString TYPE_INFORMATION = "info";
    const QString TYPE_WARNING = "warning";
    const QString TYPE_SUPPLEMENTAL = "supplemental";
    const QString TYPE_NOTE = "note";
};


enum Status
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
    MESSAGE_UNKNOWN
};

constexpr int LINT_TABLE_FILE_COLUMN = 0;
constexpr int LINT_TABLE_NUMBER_COLUMN = 1;
constexpr int LINT_TABLE_DESCRIPTION_COLUMN = 2;
constexpr int LINT_TABLE_LINE_COLUMN = 3;

constexpr char DATA_MODULE_STRING[] = "--- Module:   ";
constexpr char DATA_CPP_STRING[] = " (C++)";
constexpr char DATA_C_STRING[] = " (C)";
constexpr char DATA_LICENCE_ERROR_STRING[] = "License Error";

struct LintMessage
{
    QString file;        // File associated with message
    int line;            // Source code line number
    QString type;        // Message type ("error", "warning", "information", "supplemental")
    int number;          // Message number
    QString description; // Message description
};


using LintMessages = std::vector<LintMessage>;
using LintMessagesSet = QSet<LintMessage>;
using LintMessageGroup = std::vector<LintMessages>;

using namespace moodycamel;

class PCLintPlus : public QObject
{
    Q_OBJECT
public:
    PCLintPlus();
    PCLintPlus(const QString& lintExecutable, const QString& lintFile);

    // Set the path to the lint.exe
    void setLintExecutable(const QString& linterExecutable) noexcept;
    // Set the lint file (.lnt) to use
    void setLintFile(const QString& lintFile) noexcept;
    // Set the working directory for the lint executable
    void setWorkingDirectory(const QString& directory) noexcept;

    void setHardwareThreads(const int threads) noexcept;

    QString errorMessage() const noexcept;

    // Return path to the lint file used (.lnt)
    QString getLintFile() const noexcept;

    // So that supplemental messages are tied together with error/info/warnings
    LintMessageGroup groupLintMessages(LintMessages&& lintMessages) noexcept;

    std::vector<QString> processSourceFiles(const QByteArray& data) noexcept;
    LintMessages parseLintMessages(const QByteArray& data);

    std::vector<QByteArray> stitchModule(const QByteArray& data);

    bool parseLintFile() noexcept;

public slots:

    void slotAbortLint(bool abort) noexcept;
    void lint() noexcept;

signals:
    void signalUpdateProgress();
    void signalUpdateProgressMax(int value);
    void signalUpdateETA(int eta);
    void signalUpdateProcessedFiles();
    void signalLintComplete(const Status& lintStatus, const QString& errorMessage);
    void signalAddTreeParent(const LintMessage& parentMessage);
    void signalAddTreeChild(const LintMessage& childMessage);



private:
    QString m_lintDirectory;
    QStringList m_arguments;
    QString m_lintExecutable;
    QString m_lintFile;
    QString m_errorMessage;

    int m_hardwareThreads;
    QSet<QString> m_lintedFiles;

    // stderr has the module (file lint) progress
    // stdout has the actual data

    Status m_status;

    QFile m_stdOutFile;
    QFile m_stdErrFile;
    QFile m_remainingFile;
    int m_lintSourceFiles;

    QByteArray m_stdOut;

    std::unique_ptr<QProcess> m_process;


    void emitLintComplete() noexcept;
    void consumerThread() noexcept;
    void processModules(std::vector<QByteArray> modules);
    QString addFullFilePath(QStringView file) const noexcept;
    void processLintSourceFiles() noexcept;

    LintMessagesSet m_messageSet;

    std::atomic<bool> m_finished;
    std::unique_ptr<ReaderWriterQueue<QByteArray>> m_dataQueue;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    QFuture<void> m_future;

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
