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

#include "ProgressWindow.h"
#include "ui_ProgressWindow.h"
#include "Lint.h"
#include "Jenkins.h"

#include <QDebug>
#include <QTimer>
#include <QThread>

ProgressWindow::ProgressWindow(QWidget *parent, const QString& title) :
    QDialog(parent),
    m_ui(new Ui::ProgressWindow),
    m_elapsedTime(0),
    m_eta(5), // TODO: Make it "Calculating..."
    m_progressMax(0),
    m_currentProgress(0),
    m_fileProgressMax(0),
    m_currentFileProgress(0),
    m_aborted(false),
    m_timer(std::make_unique<QTimer>()),
    m_windowTitle(title),
    m_lintManager(std::make_unique<PCLint::LintManager>(this)),
    m_workerThread(std::make_unique<QThread>(this)),
    m_parent(static_cast<MainWindow*>(parent))
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_ui->setupUi(this);
    m_ui->lintProgressBar->setValue(0);
    m_ui->lintProgressBar->setMaximum(0);

    QObject::connect(m_timer.get(), &QTimer::timeout, this, &ProgressWindow::slotUpdateTime);
    m_timer->start(1000);
    m_lintManager->moveToThread(m_workerThread.get());
    m_workerThread->start();

    m_ui->lintGroupBox->setTitle(title);

    // ProgressWindow asks MainWindow for lint data
    // MainWindow gives ProgressWindow lint data
    // ProgressWindow gives lint data to LintManager which starts the process
    // Start lint
    // Lint object gives LintManager lint responses
    // LintManager gives ProgressWindow progress data
    // ...
    // LintManager finishes and gives ProgressWindow the data
    // ProgressWindow gives MainWindow the output data

    // ProgressWindow asks MainWindow for lint data
    QObject::connect(this, &ProgressWindow::signalGetLintData, m_parent, &MainWindow::slotGetLintData);
    // MainWindow gives ProgressWindow lint data
    QObject::connect(m_parent, &MainWindow::signalSetLintData, this, &ProgressWindow::slotGetLintData);
    // ProgressWindow gives lint data to LintManager which starts the process
    QObject::connect(this, &ProgressWindow::signalSetLintData, m_lintManager.get(), &PCLint::LintManager::slotGetLintData);
    // Start lint
    QObject::connect(this, &ProgressWindow::signalStartLint, m_lintManager.get(), &PCLint::LintManager::slotStartLint);
    // LintManager gives ProgressWindow progress data
    // ...
    // ProgressWindow gives MainWindow the output data
    QObject::connect(this, &ProgressWindow::signalLintFinished, m_parent, &MainWindow::slotLintFinished);
    QObject::connect(this, &ProgressWindow::signalLintComplete, m_parent, &MainWindow::slotLintComplete);

    emit signalGetLintData();
    emit signalStartLint();
}


void ProgressWindow::slotGetLintData(const PCLint::LintData& lintData) noexcept
{
    qDebug() << "MainWindow sends Lint data to ProgressWindow";
    emit signalSetLintData(lintData);
}

void ProgressWindow::slotUpdateProgress(int value) noexcept
{
    m_currentProgress += value;
    qDebug() << "Progress now: (" << m_currentProgress << '/' << m_progressMax << ')';
    m_ui->lintProgressBar->setValue(m_currentProgress);
}

void ProgressWindow::slotUpdateProgressMax(int value) noexcept
{
    qDebug() << "Progress max received maximum: " << value;
    // Accumulate progress
    m_progressMax += value;
    m_ui->lintProgressBar->setMaximum(m_progressMax);
}

void ProgressWindow::slotUpdateProcessedFiles(int processedFiles) noexcept
{
   m_currentFileProgress += processedFiles;
   m_ui->filesProcessed->setText(QString::number(m_currentFileProgress)+ '/' + QString::number(m_progressMax));
}

void ProgressWindow::slotUpdateETA(int eta) noexcept
{
    static int etaMax = 0;
    if (etaMax < eta)
    {
        etaMax = eta;
        m_eta = eta;
    }
}

void ProgressWindow::slotUpdateTime() noexcept
{
    m_ui->timeElapsed->setText(QDateTime::fromTime_t(m_elapsedTime++).toUTC().toString("hh:mm:ss"));
    if (m_eta >= 0)
    {
        m_ui->eta->setText(QDateTime::fromTime_t(m_eta--).toUTC().toString("hh:mm:ss"));
    }
}

void ProgressWindow::slotLintFinished(const PCLint::LintResponse& lintResponse) noexcept
{
    emit signalLintFinished(lintResponse);
}

void ProgressWindow::slotLintComplete() noexcept
{
    // Close this window first to prevent the main window
    // from minimising on close for some reason
    close();
    emit signalLintComplete();
}

ProgressWindow::~ProgressWindow()
{
    if (m_aborted == true)
    {
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION);
    }
    else
    {
        // TODO: Don't set this if there was a license error
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_windowTitle);
    }
    delete m_ui;
    m_workerThread->quit();
    auto waitComplete = m_workerThread->wait(PCLint::MAX_THREAD_WAIT);
    Q_ASSERT(waitComplete);
    qDebug() << "ProgressWindow destroyed";
}

void ProgressWindow::on_lintCancel_clicked()
{
    m_aborted = true;
    emit signalAbortLint();
}
