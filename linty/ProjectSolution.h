#ifndef PROJECTSOLUTION_H
#define PROJECTSOLUTION_H

#include <QList>
#include <QString>

class ProjectSolution
{
public:
    ProjectSolution() = default;
    ~ProjectSolution() = default;
     virtual QList<QString> buildSourceFiles(const QString& projectFileName) = 0;
};

class AtmelStudio7ProjectSolution : public ProjectSolution
{
public:

     AtmelStudio7ProjectSolution() = default;
     ~AtmelStudio7ProjectSolution() = default;
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
};

// .vcxproj
class VisualStudioProject : public ProjectSolution
{
public:

     VisualStudioProject() = default;
     ~VisualStudioProject() = default;
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
};

#endif // PROJECTSOLUTION_H
