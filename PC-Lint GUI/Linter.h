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
#include <QProcess>
#include <memory>
#include <vector>
#include <set>

namespace Lint
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
        // PC-Lint types
        const QString LINT_TYPE_ERROR = "Error";
        const QString LINT_TYPE_INFO = "Info";
        const QString LINT_TYPE_WARNING = "Warning";
        const QString LINT_TYPE_SUPPLEMENTAL = "supplemental"; // PC-Lint Plus only
    };


    enum Status
    {
        // Lint completed successfully
        LINTER_COMPLETE = 0,
        // Lint partially completed but didn't lint all files
        LINTER_PARTIAL_COMPLETE = 1,
        // Lint version unknown
        LINTER_UNSUPPORTED_VERSION = 2,
        // Lint license error
        LINTER_LICENSE_ERROR = 4,
        // Lint process error
        LINTER_PROCESS_ERROR = 8,
        // Lint process timeout
        LINTER_PROCESS_TIMEOUT = 16,
        // Lint cancelled
        LINTER_CANCEL = 32,
    };

    enum Message
    {
        MESSAGE_TYPE_ERROR = 0,
        MESSAGE_TYPE_WARNING,
        MESSAGE_TYPE_INFORMATION,
        MESSAGE_TYPE_SUPPLEMENTAL,
        MESSAGE_TYPE_UNKNOWN,
    };

    constexpr int MAX_PROCESS_CHARACTERS = 8192;
    constexpr int MAX_LINT_TIME = 10*60*1000;
    constexpr int MAX_LINT_PATH = 512;
};

typedef struct
{
    QString file;           // The file that has the problem
    QString line;           // The line that has the problem
    QString type;           // The type of the problem
    QString number;         // The message number
    QString description;    // The message description
} LintMessage;

typedef struct
{
    QString linterExecutable;
    QString lintOptionFile;
    QSet<QString> lintFiles;
} LintData;

typedef struct
{
    Lint::Status status;
    std::vector<LintMessage> lintMessages;
    int numberOfErrors;
    int numberOfWarnings;
    int numberOfInfo;
    QString errorMessage;
} LintResponse;

using LintMessageGroup = std::vector<std::vector<LintMessage>>;

class Linter : public QObject
{
    Q_OBJECT
public:
    Linter();
    void setLinterExecutable(const QString& linterExecutable) noexcept;
    void setLinterFile(const QString& lintFile) noexcept;
    void setLintFiles(const QSet<QString>& files) noexcept;

    // Gets the set of lintMessage returned after a lint
    std::vector<LintMessage> getLinterMessages() const noexcept;
    void setLinterMessages(const std::vector<LintMessage>& lintMessages) noexcept;

    // Group together lint messages (PC-Lint Plus only)
    // So that supplemental messages are tied together with error/info/warnings
    LintMessageGroup groupLinterMessages() noexcept;

    // Remove all associated messages with the given file
    void removeAssociatedMessages(const QString& file) noexcept;
    // Removes all messages with the given number
    void removeMessagesWithNumber(const QString& number) noexcept;

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


    void appendLinterMessages(const std::vector<LintMessage>& lintMessages) noexcept;
    void appendLinterErrors(int numberOfErrors) noexcept;
    void appendLinterWarnings(int numberOfWarnings) noexcept;
    void appendLinterInfo(int numberOfInfo) noexcept;

    // Lint a directory or some files
    Lint::Status lint() noexcept;

    // Return path to the lint file used (.lnt)
    QString getLinterFile() const noexcept;

public slots:
    void slotStartLint() noexcept;
    void slotGetLinterData(const LintData& lintData) noexcept;


signals:
    void signalUpdateProgress(int value);
    void signalUpdateProgressMax(int value);
    void signalUpdateETA(int eta);
    void signalUpdateProcessedFiles(int processedFiles);
    void signalLinterProgress(int value);
    void signalLintFinished(const LintResponse& lintResponse);

private:
    QString m_lintingDirectory;
    QStringList m_arguments;
    QString m_linterExecutable;
    QString m_lintFile;
    QSet<QString> m_filesToLint;
    std::vector<LintMessage> m_linterMessages;
    int m_numberOfErrors;
    int m_numberOfWarnings;
    int m_numberOfInfo;
    QString m_linterErrorMessage;
};

inline bool operator==(const LintMessage &e1, const LintMessage &e2) noexcept
{
    return (e1.number == e2.number) &&
            (e1.file == e2.file) &&
            (e1.line == e2.line) &&
            (e1.type == e2.type) &&
            (e1.description == e2.description);
}

inline uint qHash(const LintMessage &key, uint seed) noexcept
{
    return qHash(key.number + key.line, seed) ^ qHash(key.file + key.type + key.description, seed);
}
