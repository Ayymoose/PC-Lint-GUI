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
#include "PCLintPlus.h"
#include "Jenkins.h"

ProgressWindow::ProgressWindow(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ProgressWindow),
    m_elapsedTime(0),
    m_eta(0),
    m_currentProgress(0),
    m_fileProgressMax(0),
    m_currentFileProgress(0),
    m_aborted(false),
    m_timer(std::make_unique<QTimer>()),
    m_parent(static_cast<MainWindow*>(parent))
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_ui->setupUi(this);
    m_ui->lintProgressBar->setValue(0);
    m_ui->lintProgressBar->setMaximum(0);

    QObject::connect(m_timer.get(), &QTimer::timeout, this, &ProgressWindow::slotUpdateTime);
    m_ui->eta->setText("Calculating...");
}

void ProgressWindow::slotUpdateProgress() noexcept
{
    m_currentProgress++;
    m_ui->lintProgressBar->setValue(m_currentProgress);
    m_ui->filesProcessed->setText(QString::number(m_currentProgress) +
                                  '/' + QString::number(m_ui->lintProgressBar->maximum()));
}

void ProgressWindow::slotUpdateProgressMax(int maxProgress) noexcept
{
    m_timer->start(1000);
    qDebug() << "Progress max:" << maxProgress;
    m_ui->lintProgressBar->setMaximum(maxProgress);
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

void ProgressWindow::slotLintComplete(const Lint::Status& lintStatus, const QString& errorMessage) noexcept
{
    // TODO: Crash here sometimes on exec?!
    // Close this window first to prevent the main window
    // from minimising on close for some reason
    close();

    emit signalLintComplete(lintStatus, errorMessage);

    // TODO: Title bar progress for Windows only
    if (m_aborted == true)
    {
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION);
    }
    else if (lintStatus != Lint::STATUS_LICENSE_ERROR)
    {
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_ui->lintGroupBox->title());
    }
}

void ProgressWindow::setTitle(const QString& title) noexcept
{
    m_ui->lintGroupBox->setTitle(title);
    m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_ui->lintGroupBox->title());
}

ProgressWindow::~ProgressWindow()
{
    delete m_ui;
}

void ProgressWindow::on_lintCancel_clicked()
{
    // TODO: Cancel must abort right away otherwise we'll get stuck if the lint gets stuck
    m_aborted = true;
    emit signalAbortLint(true);
}
