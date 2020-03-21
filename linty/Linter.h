#ifndef LINTER_H
#define LINTER_H

#include <QString>

enum LINTER_STATUS
{
    LINTER_OK = 0,
    LINTER_EXECUTABLE_UNKNOWN,
    LINTER_TIMEOUT,
    LINTER_ERROR,
};

#define LINT_VERSION "PC-lint for C/C++ (NT) Vers. 9.00c, Copyright Gimpel Software 1985-2009\r\n"

class Linter
{
public:
    Linter() = default;
    LINTER_STATUS lint(const QString& linterExecutable, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory);
private:

};

#endif // LINTER_H
