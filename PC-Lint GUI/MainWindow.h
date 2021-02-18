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

#include <QPoint>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QMap>
#include <memory>

#include "ProgressWindow.h"
#include "Preferences.h"
#include "Linter.h"
#include "Log.h"
#include "CodeEditor.h"
#include "Highlighter.h"
#include "ModifiedFileThread.h"

class ProgressWindow;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signalUpdateTypes(int errors, int warnings, int info);

    void signalSetLinterData(const LintData& lintData);

    // Modified file signals
    void signalSetModifiedFiles(QMap<QString, ModifiedFile> modifiedFiles);
    void signalSetModifiedFile(const QString& modifiedFile, const QDateTime& dateTime);
    void signalStartMonitor();
    void signalRemoveFile(const QString& deletedFile);
    void signalKeepFile(const QString& keepFile);
public slots:
    void handleContextMenu(const QPoint& pos);
    void slotLintFinished(const LintResponse& lintResponse);
    void slotGetLinterData();
    void slotLintComplete();

private slots:

    void save();
    void exit();
    void copy();
    void cut();
    void on_actionLint_options_triggered();
    void on_actionLint_triggered();
    void on_lintTable_cellDoubleClicked(int row, int column);
    void on_actionLint_project_triggered();
    void on_aboutLinty_triggered();
    void on_actionRefresh_triggered();
    void slotFileModified(QString modifiedFile);
    void slotFileDoesntExist(const QString& deletedFile);
    void on_actionLog_triggered();

public:
    void startLint(bool lintProject);
    void startLintThread(QString title);

private:
    Ui::MainWindow* m_ui;
    std::unique_ptr<QToolBar> m_lowerToolbar;
    std::unique_ptr<QToolButton> m_buttonErrors;
    std::unique_ptr<QToolButton> m_buttonWarnings;
    std::unique_ptr<QToolButton> m_buttonInfo;
    std::unique_ptr<QAction> m_actionError;
    std::unique_ptr<QAction> m_actionWarning;
    std::unique_ptr<QAction> m_actionInfo;
    bool m_toggleError;
    bool m_toggleWarning;
    bool m_toggleInfo;
    QString m_lastProjectLoaded;
    QSet<QString> m_directoryFiles;
    std::unique_ptr<Preferences> m_preferences;
    Linter m_linter;
    std::unique_ptr<Highlighter> m_highlighter;

    std::unique_ptr<QMenu> m_lintTableMenu;
    QMap<QString, QString> m_projectLintMap;
    int m_linterStatus;

    // ModifiedFileWorker thread
    std::unique_ptr<ModifiedFileThread> m_modifiedFileWorker;

    void displayLintTable();
    bool verifyLint();
    QSet<QString> recursiveBuildSourceFileSet(const QString& directory);

};
