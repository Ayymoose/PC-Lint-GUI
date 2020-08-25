/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPoint>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QMap>
#include <memory>

#include "ProgressWindow.h"
#include "LintOptions.h"
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
    Ui::MainWindow *m_ui;
    void configureLintTable();
    void startLint(bool lintProject);
    void startLintThread(QString title);

private:
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
    std::unique_ptr<LintOptions> m_lintOptions;
    Linter m_linter;
    std::unique_ptr<Highlighter> m_highlighter;
    bool verifyLint();

    std::unique_ptr<QMenu> m_lintTableMenu;
    QMap<QString, QString> m_projectLintMap;
    int m_linterStatus;

    // ModifiedFileWorker thread
    std::unique_ptr<ModifiedFileThread> m_modifiedFileWorker;

    void displayLintTable();

    QSet<QString> recursiveBuildSourceFileSet(const QString& directory);

};

#endif // MAINWINDOW_H
