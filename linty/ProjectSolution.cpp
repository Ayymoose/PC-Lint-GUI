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
