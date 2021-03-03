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

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QPair>

namespace PCLint
{

struct ModifiedFile
{
    QDateTime lastModified;
    bool keepFile;
};

class ModifiedFileThread : public QThread
{
    Q_OBJECT
public:
    explicit ModifiedFileThread(QObject *parent = nullptr) : QThread(parent) {}
protected:
    void run() override;
public slots:
    void slotSetModifiedFiles(QMap<QString, ModifiedFile> modifiedFiles) noexcept;
    void slotSetModifiedFile(const QString& modifiedFile, const QDateTime& dateTime) noexcept;
    void slotRemoveFile(const QString& deletedFile) noexcept;
    void slotKeepFile(const QString& keepFile) noexcept;

signals:
    void signalFinished();
    void signalFileModified(const QString& modifiedFile);
    void signalFileDoesntExist(const QString& deletedFile);
private:
    QMap<QString, ModifiedFile> m_modifiedFiles;
    QMutex m_modifiedFileMutex;
};


};
