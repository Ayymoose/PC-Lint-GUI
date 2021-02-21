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
#include <QTreeWidgetItem>
#include <memory>

#include "ProgressWindow.h"
#include "Preferences.h"
#include "Linter.h"
#include "Log.h"
#include "CodeEditor.h"
#include "Highlighter.h"
#include "ModifiedFileThread.h"
#include "About.h"

class ProgressWindow;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

namespace
{
constexpr int LINT_TABLE_FILE_COLUMN = 0;
constexpr int LINT_TABLE_NUMBER_COLUMN = 1;
constexpr int LINT_TABLE_DESCRIPTION_COLUMN = 2;
constexpr int LINT_TABLE_LINE_COLUMN = 3;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signalUpdateTypes(int errors, int warnings, int info);

    void signalSetLinterData(const Lint::LintData& lintData);

    // Modified file signals
    void signalSetModifiedFiles(QMap<QString, Lint::ModifiedFile> modifiedFiles);
    void signalSetModifiedFile(const QString& modifiedFile, const QDateTime& dateTime);
    void signalStartMonitor();
    void signalRemoveFile(const QString& deletedFile);
    void signalKeepFile(const QString& keepFile);
public slots:
    void handleContextMenu(const QPoint& pos);
    void slotLintFinished(const Lint::LintResponse& lintResponse);
    void slotGetLinterData();
    void slotLintComplete();

private slots:

    void save();
    void slotFileModified(QString modifiedFile);
    void slotFileDoesntExist(const QString& deletedFile);
    void on_aboutLinty_triggered();
    void on_actionRefresh_triggered();
    void on_actionLog_triggered();
    void on_actionLintProject_triggered();
    void on_actionPreferences_triggered();
    void on_actionLint_triggered();
    void on_lintTable_itemClicked(QTreeWidgetItem *item, int column);

public:
    void startLint(bool lintProject);
    void startLintThread(QString title);
    bool filterMessageType(const QString& type) const noexcept;

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
    Lint::Linter m_linter;
    std::unique_ptr<Lint::Highlighter> m_highlighter;

    std::unique_ptr<QMenu> m_lintTableMenu;
    QMap<QString, QString> m_projectLintMap;
    int m_linterStatus;

    // ModifiedFileWorker thread
    std::unique_ptr<Lint::ModifiedFileThread> m_modifiedFileWorker;

    void displayLintTable();
    bool verifyLint();
    QSet<QString> recursiveBuildSourceFileSet(const QString& directory);
    Lint::About m_about;

};
