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

#include "Log.h"
#include <QMessageBox>

std::unique_ptr<QFile> Log::m_file;

void Log::createLogFile(const QString &file) noexcept
{
    m_file = std::make_unique<QFile>(new QFile);
    m_file->setFileName(file);
    if (!m_file->open(QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Error", "Unable to create file for logging");
    }
}

void Log::log(const QString &value) noexcept
{
    QString text = value;
    text = QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss] ") + text + "\n";
    QTextStream out(m_file.get());
    out.setCodec("UTF-8");
    if (m_file != nullptr)
    {
        out << text;
    }
}
