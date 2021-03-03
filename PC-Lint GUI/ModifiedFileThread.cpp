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

#include "ModifiedFileThread.h"
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>
#include <QMutexLocker>


// TODO: Change this to QFileSystemWatcher

namespace PCLint
{

void ModifiedFileThread::run()
{
    qDebug() << "Starting monitoring";

    // This thread monitors whether files we have loaded in the project
    // have been changed or deleted

    // If any file was deleted then we ask the user whether they want to keep the file or not
    // If they do then we try to save the file and keep it
    // If they don't want it then we remove it and update the table

    while (!isInterruptionRequested())
    {
        QMutexLocker mutexLocker(&m_modifiedFileMutex);
        QMapIterator<QString, ModifiedFile> iterator(m_modifiedFiles);
        while (iterator.hasNext())
        {
            iterator.next();

            // Monitor if file actually exists anymore
            QString currentFile = iterator.key();
            ModifiedFile modifiedFile = iterator.value();

            // Only keep the file if we want it
            if (!currentFile.isEmpty() && !QFile(currentFile).exists() && modifiedFile.keepFile)
            {
                // Notify main thread
                emit signalFileDoesntExist(currentFile);
            }
            else
            {
                QDateTime currentLastModified = modifiedFile.lastModified;

                // When file saved locally
                // Update the last modified for the saved file
                QDateTime lastModified = QFileInfo(currentFile).lastModified();

                // Monitor if a file was modified
                if (currentLastModified < lastModified)
                {
                    // Update lastModified
                    m_modifiedFiles[currentFile].lastModified = QFileInfo(currentFile).lastModified();
                    emit signalFileModified(currentFile);
                }
            }
        }
        QThread::msleep(500);
    }
    qDebug() << "Stopping monitoring";

}

void ModifiedFileThread::slotSetModifiedFiles(QMap<QString, ModifiedFile> modifiedFiles) noexcept
{
    QMutexLocker mutexLocker(&m_modifiedFileMutex);
    m_modifiedFiles = modifiedFiles;
}

void ModifiedFileThread::slotSetModifiedFile(const QString& modifiedFile, const QDateTime& dateTime) noexcept
{
    QMutexLocker mutexLocker(&m_modifiedFileMutex);
    m_modifiedFiles[modifiedFile].lastModified = dateTime;
    m_modifiedFiles[modifiedFile].keepFile = true;
}

void ModifiedFileThread::slotRemoveFile(const QString& deletedFile) noexcept
{
    QMutexLocker mutexLocker(&m_modifiedFileMutex);
    // Remove from the CodeEditor too
    m_modifiedFiles.remove(deletedFile);
}

void ModifiedFileThread::slotKeepFile(const QString& keepFile) noexcept
{
    QMutexLocker mutexLocker(&m_modifiedFileMutex);
    // Record that we want to keep this file
    m_modifiedFiles[keepFile].keepFile = false;
}

};
