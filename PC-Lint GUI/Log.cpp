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

void customMessageHandler(QtMsgType type, const QMessageLogContext&, const QString &msg)
{

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1]").arg(dt);

   switch (type)
   {
       case QtInfoMsg:
          txt += QString("[Info] %1").arg(msg);
          break;
      case QtDebugMsg:
         txt += QString("[Debug] %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("[Warning] %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("[Critical] %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("[Fatal] %1").arg(msg);
         break;
   }

   QFile outFile(Lint::LOG_FILENAME);
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);
   if (!outFile.isOpen())
   {
       QMessageBox::critical(nullptr, "Error", "Could not create log file: " + outFile.errorString());
       return;
   }

   QTextStream textStream(&outFile);
   textStream << txt << endl;
}

