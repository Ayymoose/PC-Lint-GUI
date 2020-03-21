#ifndef LINTER_H
#define LINTER_H

#include <QString>

enum LINTER_STATUS
{
    LINTER_OK = 0,
    LINTER_EXECUTABLE_UNKNOWN,
    LINTER_ERROR,
};

class Linter
{
public:
    Linter() = default;
    LINTER_STATUS lint(const QString& linterExecutablePath, const QString& linterFilePath, const QString& linterLintOptions, const QString& linterDirectory);
private:

};

#endif // LINTER_H
