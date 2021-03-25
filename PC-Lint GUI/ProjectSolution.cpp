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

#include "ProjectSolution.h"
#include <windows.h>
#include "Log.h"

namespace PCLint
{


namespace
{
    const QString XML_NODE_COMPILE = "Compile";
    const QString XML_ATTRIBUTE_INCLUDE = "Include";
    const QString XML_NODE_CLCOMPILE = "ClCompile";

    // Visual studio solution files
    const QString SOLUTION_LINE_PROJECT = "Project";
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
        ProjectSolution::readerHasError(xmlReader);
    }
    else
    {
        auto const errorMessage = "Couldn't open " + projectFileName + " because " + projectFile.errorString();
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
                auto const name = xmlReader.name();
                if (name == XML_NODE_CLCOMPILE)
                {
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    auto const include = attrs.value(XML_ATTRIBUTE_INCLUDE).toString();

                    // Does this tag have Include attributes?
                    if (include.length() == 0)
                    {
                        continue;
                    }

                    // Add recognised suffices (Does VS accept only C++ files?)
                    auto const fileSuffix = QFileInfo(include).suffix();
                    Q_ASSERT(fileSuffix.length() > 0);

                    if (ProjectSolution::knownFileExtensions(fileSuffix))
                    {
                        // Append the project path so we can get the canonical file path
                        auto const sourceFile = QFileInfo(projectPath + R"(\)" + include).canonicalFilePath();
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
        ProjectSolution::readerHasError(xmlReader);
    }
    else
    {
        auto const errorMessage = "Couldn't open " + projectFileName + " because " + projectFile.errorString();
        qCritical() << errorMessage;
        throw std::runtime_error(errorMessage.toStdString());
    }

    return sourceFiles;
}

void VisualStudioProjectSolution::setDirectory(const QString &directory) noexcept
{
    m_directory = directory;
}

QSet<QString> VisualStudioProjectSolution::buildSourceFiles(const QString &projectFileName)
{
    QSet<QString> sourceFiles;
    VisualStudioProject vsProject;

    // Directory must be set before calling this function
    Q_ASSERT(m_directory.length());

    int maxProjects = 0;

    QFile projectFile(projectFileName);
    if (projectFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&projectFile);
       while (!in.atEnd())
       {
          auto const line = in.readLine();

          // Basic VS studio parsing
          // If line begins with "Project"
          // Split by comma
          // Extra 1st parameter
          // Trim to give VC project file

          // E.g of data
          // Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "ConsoleApplication1", "ConsoleApplication1.vcxproj", "{188E5CAB-AA8D-4586-AE17-6BB964C21E48}"

          if (line.startsWith(SOLUTION_LINE_PROJECT))
          {
              auto const split = line.split(R"(")");
              Q_ASSERT(split.size() >= 5);
              auto const projectUnclean = split[5];

              qInfo() << "Found " << projectUnclean;

              //if (maxProjects < 10)
              {
                  try
                  {
                      // Assemble each solution in turn
                      auto const solutionFiles =  vsProject.buildSourceFiles(m_directory + '/' + projectUnclean);
                      maxProjects++;

                      // TODO: Redo this inneficient way to appending QList together
                      for (auto const& file : solutionFiles)
                      {
                          sourceFiles.insert(file);
                      }
                  }
                  catch (const std::logic_error& e)
                  {
                      qCritical() << QString(e.what());
                  }
                  catch (const std::runtime_error& e)
                  {
                      qCritical() << QString(e.what());
                  }


              }
          }


       }
       projectFile.close();
    }
    else
    {
        auto const errorMessage = "Couldn't open " + projectFileName;
        qCritical() << errorMessage;
        throw std::runtime_error(errorMessage.toStdString());
    }

    return sourceFiles;
}

QSet<QString> QtSolution::buildSourceFiles(const QString &projectFileName)
{
    QSet<QString> sourceFiles;
    QFile projectFile(projectFileName);
    if (projectFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&projectFile);
       while (!in.atEnd())
       {
          auto const line = in.readLine();

          // Look for C++ files
          if (line.contains(".cpp"))
          {
              auto sourceFile = line.trimmed();
              sourceFile = sourceFile.mid(0, sourceFile.indexOf(".cpp")+4);
              sourceFiles.insert(sourceFile);
          }
       }
    }
    else
    {
        auto const errorMessage = "Couldn't open " + projectFileName;
        qCritical() << errorMessage;
        throw std::runtime_error(errorMessage.toStdString());
    }
    return sourceFiles;
}

};
