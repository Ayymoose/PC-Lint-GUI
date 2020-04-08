#ifndef PROJECTSOLUTION_H
#define PROJECTSOLUTION_H

#include <QList>
#include <QString>

class ProjectSolution
{
public:
     virtual QList<QString> buildSourceFiles(const QString& projectFileName) = 0;
};

class AtmelStudio7ProjectSolution : public ProjectSolution
{
public:

     AtmelStudio7ProjectSolution() = default;
     ~AtmelStudio7ProjectSolution() = default;
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
};

#endif // PROJECTSOLUTION_H
