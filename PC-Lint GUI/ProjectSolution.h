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

#include <QList>
#include <QString>
#include <QXmlStreamReader>

namespace Lint
{

class ProjectSolution
{
public:
    ProjectSolution() = default;
    virtual ~ProjectSolution() = default;
    virtual QSet<QString> buildSourceFiles(const QString& projectFileName) = 0;
    virtual void setDirectory(const QString&) = 0;

    static bool knownFileExtensions(const QString& extension)
    {
        const QStringList extensions =
        {
            "c","cc","cpp","c++","cp","cxx"
        };
        return extensions.contains(extension, Qt::CaseInsensitive);
    }

    static void readerHasError(const QXmlStreamReader& xmlReader)
    {
        if(xmlReader.hasError())
        {
            qCritical() << "XML parser error in " << xmlReader.name();
            qCritical() << "Error Type:       " << QString(xmlReader.error());
            qCritical() << "Error String:     " << xmlReader.errorString();
            qCritical() << "Line Number:      " << QString::number(xmlReader.lineNumber());
            qCritical() << "Column Number:    " << QString::number(xmlReader.columnNumber());
            qCritical() << "Character Offset: " << QString::number(xmlReader.characterOffset());
            throw std::logic_error("XML parser error");
        }
    }


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

};
