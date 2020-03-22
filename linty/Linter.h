#ifndef LINTER_H
#define LINTER_H

#include <QString>
#include <QCoreApplication>

enum LINTER_STATUS
{
    LINTER_OK = 0,
    LINTER_UNSUPPORTED_VERSION,
    LINTER_TIMEOUT,
    LINTER_ERROR,
};

typedef struct
{
    QString file;
    QString line;
    QString type;
    QString code;
    QString description;
} lintMessage;

//Will be in a list of supported versions later
#define LINT_VERSION "PC-lint for C/C++ (NT) Vers. 9.00c, Copyright Gimpel Software 1985-2009\r\n"

enum SUPPORTED_LINTERS
{
    //TODO: Finish
    PC_LINT_V9 = 0 // PC-lint for C/C++ (NT) Vers. 9.00c, Copyright Gimpel Software 1985-2009\r\n",
};


// XML related definitions
#define XML_DOC "doc"
#define XML_MESSAGE "message"
#define XML_FILE "file"
#define XML_LINE "line"
#define XML_TYPE "type"
#define XML_CODE "code"
#define XML_DESCRIPTION "description"




class Linter
{
        Q_DECLARE_TR_FUNCTIONS(Linter)
public:
    Linter() = default;
    LINTER_STATUS lint(const QString& linterExecutable, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory, QList<lintMessage>& lintOutputMessages);
private:

};

#endif // LINTER_H
