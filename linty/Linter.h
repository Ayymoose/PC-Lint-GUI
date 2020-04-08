#ifndef LINTER_H
#define LINTER_H

#include <QString>
#include <QMetaType>
#include <QSet>
#include <QObject>

enum LINTER_STATUS
{
    LINTER_OK = 0,
    LINTER_UNSUPPORTED_VERSION,
    LINTER_FAIL,
    LINTER_ERROR,
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
    QString file;
    QString line;
    QString type;
    QString code;
    QString description;
} lintMessage;

// XML related definitions
#define XML_DOC "doc"
#define XML_MESSAGE "message"
#define XML_FILE "file"
#define XML_LINE "line"
#define XML_TYPE "type"
#define XML_CODE "code"
#define XML_DESCRIPTION "description"

// Linter output related
#define TYPE_ERROR "Error"
#define TYPE_INFORMATION "Info"
#define TYPE_WARNING "Warning"

#define COMPARE_TYPE(string, type) (!QString::compare(string, type, Qt::CaseInsensitive))

class Linter : public QObject
{
    Q_OBJECT
public:
    Linter();
    void setLinterExecutable(const QString& linterExecutable);
    void setLinterFile(const QString& lintFile);
    void setLinterDirectory(const QString& linterDirectory);
    void setLintFiles(const QList<QString>& files);

    // Gets the set of lintMessage returned after a lint
    QSet<lintMessage> getLinterMessages() const;

    int numberOfErrors() const;
    int numberOfWarnings() const;
    int numberOfInfo() const;

    // Lint a directory or some files
    LINTER_STATUS lint();

    QString getLintingDirectory() const;
    QString getLinterExecutable() const;

signals:
    void signalUpdateProgress(int value);
    void signalUpdateProgressMax(int value);
    void signalUpdateStatus(QString status);

private:
    QString m_lintingDirectory;
    QSet<QString> m_supportedVersions;
    QStringList m_arguments;
    QString m_linterExecutable;
    QString m_lintFile;
    QList<QString> m_filesToLint;
    QSet<lintMessage> m_linterMessages;
    int m_numberOfErrors;
    int m_numberOfWarnings;
    int m_numberOfInfo;
};

inline bool operator==(const lintMessage &e1, const lintMessage &e2)
{
    return (e1.code == e2.code) && (e1.file == e2.file) && (e1.line == e2.line) && (e1.type == e2.type) && (e1.description == e2.description);
}

inline uint qHash(const lintMessage &key, uint seed)
{
    return qHash(key.code + key.line, seed) ^ qHash(key.file + key.type + key.description, seed);
}


#endif // LINTER_H
