#pragma once

#include <QString>
#include <QMetaType>
#include <QSet>
#include <QObject>
#include <QProcess>
#include <memory>

#define MAX_PROCESS_CHARACTERS 8192
#define MAX_LINT_TIME 10*60*1000

enum LINTER_STATUS
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

enum MESSAGE_TYPE
{
    MESSAGE_TYPE_ERROR = 0,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_INFORMATION,
    MESSAGE_TYPE_UNKNOWN,
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
    QList<QString> lintFiles;
} LintData;

typedef struct
{
    LINTER_STATUS status;
    QSet<LintMessage> lintMessages;
    int numberOfErrors;
    int numberOfWarnings;
    int numberOfInfo;
} LintResponse;

// XML tags
#define XML_TAG_DOC_OPEN "<doc>"
#define XML_TAG_DOC_CLOSED "</doc>"
#define XML_TAG_MESSAGE_OPEN "<m>"
#define XML_TAG_DESCRIPTION_CLOSED "</d>"

// XML elements
#define XML_ELEMENT_DOC "doc"
#define XML_ELEMENT_FILE "f"
#define XML_ELEMENT_LINE "l"
#define XML_ELEMENT_MESSAGE_TYPE "t"
#define XML_ELEMENT_MESSAGE_NUMBER "n"
#define XML_ELEMENT_MESSAGE "m"
#define XML_ELEMENT_DESCRIPTION "d"

// PC-Lint types
#define LINT_TYPE_ERROR "Error"
#define LINT_TYPE_INFO "Info"
#define LINT_TYPE_WARNING "Warning"

class Linter : public QObject
{
    Q_OBJECT
public:
    Linter();
    ~Linter();
    void setLinterExecutable(const QString& linterExecutable);
    void setLinterFile(const QString& lintFile);
    void setLintFiles(const QList<QString>& files);

    // Gets the set of lintMessage returned after a lint
    QSet<LintMessage> getLinterMessages() const;
    void setLinterMessages(const QSet<LintMessage>& lintMessages);

    // Remove all associated messages with the given file
    void removeAssociatedMessages(const QString& file);
    // Removes all messages with the given number
    void removeMessagesWithNumber(const QString& number);

    // Clear all messages and information
    void resetLinter();

    int numberOfErrors() const;
    int numberOfWarnings() const;
    int numberOfInfo() const;

    void setNumberOfErrors(int numberOfErrors);
    void setNumberOfWarnings(int numberOfWarnings);
    void setNumberOfInfo(int numberOfInfo);

    void appendLinterMessages(const QSet<LintMessage>& lintMessages);
    void appendLinterErrors(int numberOfErrors);
    void appendLinterWarnings(int numberOfWarnings);
    void appendLinterInfo(int numberOfInfo);

    // Lint a directory or some files
    LINTER_STATUS lint();

    QString getLintingDirectory() const;
    QString getLinterExecutable() const;

public slots:
    void slotStartLint();
    void slotGetLinterData(const LintData& lintData);


signals:
    void signalUpdateProgress(int value);
    void signalUpdateProgressMax(int value);
    void signalUpdateETA(int eta);
    void signalUpdateProcessedFiles(int processedFiles);


    //
    void signalLinterProgress(int value);
    void signalLintFinished(const LintResponse& lintResponse);

private:
    QString m_lintingDirectory;
    QSet<QString> m_supportedVersions;
    QStringList m_arguments;
    QString m_linterExecutable;
    QString m_lintFile;
    QList<QString> m_filesToLint;
    QSet<LintMessage> m_linterMessages;
    int m_numberOfErrors;
    int m_numberOfWarnings;
    int m_numberOfInfo;
};

inline bool operator==(const LintMessage &e1, const LintMessage &e2)
{
    return (e1.number == e2.number) && (e1.file == e2.file) && (e1.line == e2.line) && (e1.type == e2.type) && (e1.description == e2.description);
}

inline uint qHash(const LintMessage &key, uint seed)
{
    return qHash(key.number + key.line, seed) ^ qHash(key.file + key.type + key.description, seed);
}
