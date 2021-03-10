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

#include "Lint.h"
#include "MainWindow.h"
#include "LintManager.h"

#include <QTime>
#include <QDialog>
#include <QSet>
#include <vector>
#include <memory>


namespace PCLint
{
class LintManager;
}

class MainWindow;

namespace Ui
{
class ProgressWindow;
}

class ProgressWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressWindow(QWidget *parent = nullptr, const QString& title = "");
    ~ProgressWindow();

public slots:
    void slotUpdateProgress(int value) noexcept;
    void slotUpdateProgressMax(int value) noexcept;
    void slotUpdateETA(int eta) noexcept;
    void slotUpdateProcessedFiles(int processedFiles) noexcept;
    void slotLintFinished(const PCLint::LintResponse& lintResponse) noexcept;


    void slotLintComplete(const PCLint::LintStatus& lintStatus, const QString& errorMessage) noexcept;

    void slotGetLintData(const PCLint::LintData& lintData) noexcept;


    void slotProcessLintMessages(const PCLint::LintResponse& lintResponse) noexcept;

private slots:
    void on_lintCancel_clicked();
    void slotUpdateTime() noexcept;

signals:
    void signalLintComplete(const PCLint::LintStatus& lintStatus, const QString& errorMessage);


    void signalLintFinished(const PCLint::LintResponse& lintResponse);
    void signalAbortLint();
    void signalStartLintManager();


    void signalProcessLintMessages(const PCLint::LintResponse& lintResponse);


    void signalStartLint();
    void signalGetLintData();
    void signalSetLintData(const PCLint::LintData& lintData);
private:
    Ui::ProgressWindow *m_ui;
    int m_elapsedTime;
    int m_eta;
    int m_progressMax;
    int m_currentProgress;
    int m_fileProgressMax;
    int m_currentFileProgress;
    bool m_aborted;
    std::unique_ptr<QTimer> m_timer;
    QString m_windowTitle;
    std::unique_ptr<PCLint::LintManager> m_lintManager;
    //std::unique_ptr<QThread> m_workerThread;
    MainWindow* m_parent;
};
