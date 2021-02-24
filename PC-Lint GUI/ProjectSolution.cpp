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

#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "ProjectSolution.h"
#include <windows.h>
#include "Log.h"
#include <stdexcept>

namespace Lint
{


namespace
{
    const QString XML_NODE_COMPILE = "Compile";
    const QString XML_ATTRIBUTE_INCLUDE = "Include";
};

QSet<QString> AtmelStudio7ProjectSolution::buildSourceFiles(const QString& projectFileName)
{
    QSet<QString> sourceFiles;

    // Get the file path for the project
    auto const projectPath = QFileInfo(projectFileName).absolutePath();

    // XML path to go:

    // Project
    // - ItemGroup
    // -- Compile Include

    QXmlStreamReader xmlReader;
    QFile projectFile(projectFileName);
    if (projectFile.open(QIODevice::ReadOnly))
    {
        xmlReader.setDevice(&projectFile);
        while (!xmlReader.atEnd() && !xmlReader.hasError())
        {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader.readNext();

            // If token is just StartDocument - go to next
            if (token == QXmlStreamReader::StartDocument)
            {
                continue;
            }

            // If token is StartElement - read it
            if (token == QXmlStreamReader::StartElement)
            {
                auto const name = xmlReader.name();
                if (name == XML_NODE_COMPILE)
                {
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    auto const include = attrs.value(XML_ATTRIBUTE_INCLUDE).toString();

                    // Add recognised file extensions
                    auto const fileSuffix = QFileInfo(include).suffix();
                    Q_ASSERT(fileSuffix.length() > 0);
                    if (ProjectSolution::knownFileExtensions(fileSuffix))
                    {
                        // Append the project path so we can get the canonical file path
                        auto const canonicalPath = projectPath + R"(\)" + include;
                        auto const sourceFile = QFileInfo(canonicalPath).filePath();

                        // The file must be relative to the solution
                        Q_ASSERT(QFileInfo(sourceFile).exists());

                        sourceFiles.insert(sourceFile);
                    }

                }
                else
                {
                    continue;
                }

            }

        }
        projectFile.close();
        if(xmlReader.hasError())
        {
            qCritical() << "XML parser error in " << projectFileName;
            qCritical() << "Error Type:       " << QString(xmlReader.error());
            qCritical() << "Error String:     " << xmlReader.errorString();
            qCritical() << "Line Number:      " << QString::number(xmlReader.lineNumber());
            qCritical() << "Column Number:    " << QString::number(xmlReader.columnNumber());
            qCritical() << "Character Offset: " << QString::number(xmlReader.characterOffset());
            throw std::logic_error("XML parser error!");
        }
    }
    else
    {
        auto const errorMessage = "File open error: " + projectFile.errorString();
        qCritical() << errorMessage;
        throw std::runtime_error(errorMessage.toStdString());
    }

    return sourceFiles;
}

QSet<QString> VisualStudioProject::buildSourceFiles(const QString& projectFileName)
{
    QSet<QString> sourceFiles;

    // Get the file path for the project
    QString projectPath = QFileInfo(projectFileName).absolutePath();

    // Project
    // - ItemGroup
    // -- ClCompile Include
    QXmlStreamReader xmlReader;
    QFile projectFile(projectFileName);
    if (projectFile.open(QIODevice::ReadOnly))
    {
        xmlReader.setDevice(&projectFile);
        while (!xmlReader.atEnd() && !xmlReader.hasError())
        {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader.readNext();

            // If token is just StartDocument - go to next
            if (token == QXmlStreamReader::StartDocument)
            {
                continue;
            }

            // If token is StartElement - read it
            if (token == QXmlStreamReader::StartElement)
            {
                QStringRef name = xmlReader.name();
                if (name == "ClCompile")
                {
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    QString include = attrs.value("Include").toString();

                    // Add only CPP files
                    if (QFileInfo(include).suffix() == "cpp")
                    {
                        // Append the project path so we can get the canonical file path
                        QString sourceFile = QFileInfo(projectPath + "\\" + include).canonicalFilePath();
                        Q_ASSERT(sourceFile.length() > 0);

                        sourceFiles.insert(sourceFile);
                    }

                }
                else
                {
                    continue;
                }

            }

        }
        projectFile.close();
        if(xmlReader.hasError())
        {
            qCritical() << "XML parser error in " << projectFileName;
            qCritical() << "Error Type:       " << QString(xmlReader.error());
            qCritical() << "Error String:     " << xmlReader.errorString();
            qCritical() << "Line Number:      " << QString::number(xmlReader.lineNumber());
            qCritical() << "Column Number:    " << QString::number(xmlReader.columnNumber());
            qCritical() << "Character Offset: " << QString::number(xmlReader.characterOffset());
            throw std::logic_error("XML parser error!");
        }
    }
    else
    {
        qCritical() << "Unable to open file: " << projectFileName;
        throw std::logic_error("Unable to open file: " + projectFileName.toStdString());
    }

    return sourceFiles;
}

void VisualStudioProjectSolution::setDirectory(const QString &directory)
{
    m_directory = directory;
}

QSet<QString> VisualStudioProjectSolution::buildSourceFiles(const QString &projectFileName)
{
    QSet<QString> sourceFiles;
    VisualStudioProject vsProject;

    int maxProjects = 0;

    QFile inputFile(projectFileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();

          // Basic VS studio parsing
          // If line begins with "Project"
          // Split by comma
          // Extra 1st parameter
          // Trim to give VC project file

          // E.g of data
          // Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "Address Manager", "Address manager\Project\Windows XP, win32 (1)\addressmanager.vcxproj", "{4FA38C84-2DC0-4A02-B31F-A02A4B36480C}"

          if (line.startsWith("Project"))
          {
              QStringList split = line.split("\"");
              QString  projectUnclean = split[5];
              qDebug() << projectUnclean;

              //if (maxProjects < 10)
              {
                  try
                  {
                      QSet<QString> solutionFiles =  vsProject.buildSourceFiles(m_directory + "/" + projectUnclean);
                      maxProjects++;

                      // TODO: Redo this inneficient way to appending QList together
                      for (auto const& file : solutionFiles)
                      {
                          sourceFiles.insert(file);
                      }
                  } catch (const std::logic_error& e)
                  {
                      qCritical() << QString(e.what());
                  }


              }
          }


       }
       inputFile.close();
    }
    else
    {
        throw std::logic_error("Unable to open " + projectFileName.toStdString() + " for reading!");
    }

    return sourceFiles;
}

};
