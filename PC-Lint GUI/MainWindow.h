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
#include <QObject>
#include <QProcess>
#include <QLoggingCategory>
#include <QApplication>
#include <QScreen>
#include <QStandardItemModel>

#include "ProgressWindow.h"
#include "Preferences.h"
#include "PCLintPlus.h"
#include "Log.h"
#include "CodeEditor.h"
#include "Highlighter.h"
#include "About.h"

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

public slots:
    void slotLintComplete(const Lint::Status& lintStatus, const QString& errorMessage) noexcept;


    void slotAddTreeParent(const Lint::LintMessage& parentMessage) noexcept;
    void slotAddTreeChild(const Lint::LintMessage& childMessage) noexcept;


private slots:
    void save();
    void on_aboutLint_triggered();
    void on_actionRefresh_triggered();
    void on_actionLog_triggered();
    void on_actionPreferences_triggered();
    void on_actionLint_triggered();
    //void on_m_lintTree_itemClicked(QTreeWidgetItem *item, int column);

public:
    void startLint(QString title);

private:
    Ui::MainWindow* m_ui;
    std::unique_ptr<QToolBar> m_lowerToolbar;
    std::unique_ptr<QToolButton> m_buttonErrors;
    std::unique_ptr<QToolButton> m_buttonWarnings;
    std::unique_ptr<QToolButton> m_buttonInformation;
    std::unique_ptr<QAction> m_actionError;
    std::unique_ptr<QAction> m_actionWarning;
    std::unique_ptr<QAction> m_actionInformation;
    bool m_toggleError;
    bool m_toggleWarning;
    bool m_toggleInformation;
    QString m_lastProjectLoaded;
    std::unique_ptr<Preferences> m_preferences;
    std::unique_ptr<Lint::Highlighter> m_highlighter;

    std::unique_ptr<QMenu> m_m_lintTreeMenu;

    int m_numberOfErrors;
    int m_numberOfWarnings;
    int m_numberOfInformations;

    void clearTreeNodes() noexcept;
    void applyTreeFilter(bool filter, const QString& type) const noexcept;

    bool filterMessageType(const QString& type) const noexcept;


    bool checkLint();
    Lint::About m_about;

    void setupLintTree() noexcept;



    std::unique_ptr<Lint::PCLintPlus> m_lint;
    std::unique_ptr<ProgressWindow> m_progressWindow;

    auto createTreeNodes(const Lint::LintMessage& message) noexcept;
    QStandardItemModel m_treeModel;
    QStandardItem* m_parent;


};
