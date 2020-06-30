#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "ProjectSolution.h"
#include <windows.h>
#include "Log.h"
#include <stdexcept>

QList<QString> AtmelStudio7ProjectSolution::buildSourceFiles(const QString& projectFileName)
{
    QList<QString> sourceFiles;

    // Get the file path for the project
    QString projectPath = QFileInfo(projectFileName).absolutePath();

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
                QStringRef name = xmlReader.name();
                if (name == "Compile")
                {
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    QString include = attrs.value("Include").toString();

                    // Add only C files
                    // Can you build CPP projects with Atmel studio?
                    if (QFileInfo(include).suffix() == "c")
                    {
                        // Append the project path so we can get the canonical file path
                        QString sourceFile = QFileInfo(projectPath + "\\" + include).canonicalFilePath();
                        if (sourceFile.length() >= MAX_PATH)
                        {
                            DEBUG_LOG("### Source file name longer than " + QString(MAX_PATH) + " characters");
                        }
                        else
                        {
                            sourceFiles.append(sourceFile);
                        }
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
            DEBUG_LOG("### XML parser error in " + projectFileName);
            DEBUG_LOG("Error Type:       " + QString(xmlReader.error()));
            DEBUG_LOG("Error String:     " + xmlReader.errorString());
            DEBUG_LOG("Line Number:      " + QString::number(xmlReader.lineNumber()));
            DEBUG_LOG("Column Number:    " + QString::number(xmlReader.columnNumber()));
            DEBUG_LOG("Character Offset: " + QString::number(xmlReader.characterOffset()));
            throw std::logic_error("XML parser error!");
        }
    }
    else
    {
        DEBUG_LOG("### Unable to open file: " + projectFileName);
        throw std::logic_error("Unable to open file: " + projectFileName.toStdString());
    }

    return sourceFiles;
}

QList<QString> VisualStudioProject::buildSourceFiles(const QString& projectFileName)
{
    QList<QString> sourceFiles;

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
                        if (sourceFile.length() >= MAX_PATH)
                        {
                            DEBUG_LOG("### Source file name longer than " + QString(MAX_PATH) + " characters");
                        }
                        else
                        {
                            sourceFiles.append(sourceFile);
                        }
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
            DEBUG_LOG("### XML parser error in " + projectFileName);
            DEBUG_LOG("Error Type:       " + QString(xmlReader.error()));
            DEBUG_LOG("Error String:     " + xmlReader.errorString());
            DEBUG_LOG("Line Number:      " + QString::number(xmlReader.lineNumber()));
            DEBUG_LOG("Column Number:    " + QString::number(xmlReader.columnNumber()));
            DEBUG_LOG("Character Offset: " + QString::number(xmlReader.characterOffset()));
            throw std::logic_error("XML parser error!");
        }
    }
    else
    {
        DEBUG_LOG("### Unable to open file: " + projectFileName);
        throw std::logic_error("Unable to open file: " + projectFileName.toStdString());
    }

    return sourceFiles;
}

void VisualStudioProjectSolution::setDirectory(const QString &directory)
{
    m_directory = directory;
}

QList<QString> VisualStudioProjectSolution::buildSourceFiles(const QString &projectFileName)
{
    QList<QString> sourceFiles;
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
                      QList<QString> solutionFiles =  vsProject.buildSourceFiles(m_directory + "/" + projectUnclean);
                      maxProjects++;

                      // TODO: Redo this inneficient way to appending QList together
                      for (auto const& file : solutionFiles)
                      {
                          sourceFiles.append(file);
                      }
                  } catch (const std::logic_error& e)
                  {
                      DEBUG_LOG("### " + QString(e.what()));
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
