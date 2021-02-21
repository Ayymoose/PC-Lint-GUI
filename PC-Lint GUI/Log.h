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
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <memory>
#include <QtDebug>

#define DEBUG_LOG(MESSAGE) qDebug() << (MESSAGE);/* Log::log(MESSAGE);*/

// TODO: Date log this file with "PC-Lint GUI_dd-mm-yyyy"
#define LOG_FILENAME "PC-Lint GUI_log.txt"

namespace Lint
{

class Log : public QObject
{
    Q_OBJECT
public:
    static void createLogFile(const QString &file) noexcept;
    static void log(const QString &message) noexcept;
    Log() = delete;
private:
    static std::unique_ptr<QFile> m_file;
};

};
