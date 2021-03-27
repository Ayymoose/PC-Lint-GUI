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

#include "About.h"

namespace Lint
{

void About::display() const noexcept
{
    QMessageBox versionMessageBox;
    versionMessageBox.setIcon(QMessageBox::Information);
    versionMessageBox.addButton("Copy to clipboard", QMessageBox::AcceptRole);
    versionMessageBox.setWindowTitle("Information");

    auto const applicationInfo = QString(
                "Qt version: %1\n"
                "Build version: %2 %3 %4\n"
                "Build date: %5\n"
                "Build commit: %6\n"
                "Build compiler: %7 %8").arg(
                QT_VERSION_STR,
                APPLICATION_NAME,
                BUILD_VERSION,
                BUILD_ARCHITECTURE,
                BUILD_DATE,
                BUILD_COMMIT,
                COMPILER_NAME,
                COMPILER_VERSION);

    versionMessageBox.setText(applicationInfo);
    switch (versionMessageBox.exec())
    {
    case QMessageBox::AcceptRole:
        QApplication::clipboard()->setText(applicationInfo);
        break;
    default:
        break;
    }
}


};
