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
    virtual void setDirectory(const QString& directory) = 0;
};

class AtmelStudio7ProjectSolution : public ProjectSolution
{
public:

     AtmelStudio7ProjectSolution() = default;
     ~AtmelStudio7ProjectSolution() = default;
     void setDirectory(const QString& directory) override {}
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
};

// .vcxproj
class VisualStudioProject : public ProjectSolution
{
public:

     VisualStudioProject() = default;
     ~VisualStudioProject() = default;
     void setDirectory(const QString& directory) override {}
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
};

// .slns
class VisualStudioProjectSolution : public ProjectSolution
{
public:

     VisualStudioProjectSolution() = default;
     ~VisualStudioProjectSolution() = default;
     void setDirectory(const QString& directory) override;
     QList<QString> buildSourceFiles(const QString& projectFileName) override;
private:
     QString m_directory;
};

#endif // PROJECTSOLUTION_H
