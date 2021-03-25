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

namespace PCLint
{

void About::display() const noexcept
{
    QMessageBox versionMessageBox(nullptr);
    versionMessageBox.setIcon(QMessageBox::Information);
    versionMessageBox.addButton("Copy to clipboard", QMessageBox::AcceptRole);
    versionMessageBox.setWindowTitle("Information");
    char buildCompiler[64];

    sprintf(buildCompiler,"%s %s\n", COMPILER_NAME, COMPILER_VERSION);

    QString applicationInfo =
            "Qt version: " QT_VERSION_STR "\n"
            "Build version: " BUILD_ARCHITECTURE " " APPLICATION_NAME " " BUILD_VERSION "\n"
            "Build date: " BUILD_DATE "\n"
            "Build commit: " BUILD_COMMIT "\n"
            "Build compiler: " + QString(buildCompiler) + "\n";

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
