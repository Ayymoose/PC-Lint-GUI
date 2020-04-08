#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "ProjectSolution.h"
#include <windows.h>
#include "Log.h"

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

                    if (QFileInfo(include).suffix() == "c")
                    {
                        // Append the project path so we can get the canonical file path
                        QString sourceFile = QFileInfo(projectPath + "\\" + include).canonicalFilePath();
                        sourceFiles.append(sourceFile);
                        if (sourceFile.length() >= MAX_PATH)
                        {
                            DEBUG_LOG("### Source file name longer than " + QString(MAX_PATH) + " characters");
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
            qDebug() << "Error Type:       " << xmlReader.error();
            qDebug() << "Error String:     " << xmlReader.errorString();
            qDebug() << "Line Number:      " << xmlReader.lineNumber();
            qDebug() << "Column Number:    " << xmlReader.columnNumber();
            qDebug() << "Character Offset: " << xmlReader.characterOffset();
        }
    }
    else
    {
        qDebug() << "Unable to open file for reading";
    }

    return sourceFiles;
}
