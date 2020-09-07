#pragma once

#include <QList>
#include <QString>

class ProjectSolution
{
public:
    ProjectSolution() = default;
    ~ProjectSolution() = default;
    virtual QSet<QString> buildSourceFiles(const QString& projectFileName) = 0;
    virtual void setDirectory(const QString&) = 0;
};

class AtmelStudio7ProjectSolution : public ProjectSolution
{
public:
     void setDirectory(const QString&) override {}
     QSet<QString> buildSourceFiles(const QString& projectFileName) override;
};

// .vcxproj
class VisualStudioProject : public ProjectSolution
{
public:
     void setDirectory(const QString&) override {}
     QSet<QString> buildSourceFiles(const QString& projectFileName) override;
};

// .slns
class VisualStudioProjectSolution : public ProjectSolution
{
public:

     void setDirectory(const QString&) override;
     QSet<QString> buildSourceFiles(const QString& projectFileName) override;
private:
     QString m_directory;
};
