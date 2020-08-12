#include "ProgressWindow.h"
#include "ui_ProgressWindow.h"
#include "Linter.h"
#include "Jenkins.h"

#include <QDebug>
#include <QTimer>
#include <QThread>

ProgressWindow::ProgressWindow(QWidget *parent, const QString& title) :
    QDialog(parent),
    ui(new Ui::ProgressWindow)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_elapsedTime = 0;
    m_eta = 5;
    ui->setupUi(this);
    ui->lintProgressBar->setValue(0);
    ui->lintProgressBar->setMaximum(0);
    m_timer = std::make_unique<QTimer>();

    connect(m_timer.get(), &QTimer::timeout, this, &ProgressWindow::slotUpdateTime);
    m_timer->start(1000);
    m_windowTitle = title;
    m_progressMax = 0;
    m_currentProgress = 0;
    m_fileProgressMax = 0;
    m_currentFileProgress = 0;
    m_parent = dynamic_cast<MainWindow*>(parent);
    m_aborted = false;
    m_lintThreadManager = std::make_unique<LintThreadManager>(this);

    m_workerThread = std::make_unique<QThread>(this);
    m_lintThreadManager->moveToThread(m_workerThread.get());
    m_workerThread->start();

    // Signal lint thread manager to start linting
    connect(this, &ProgressWindow::signalStartLintManager, m_lintThreadManager.get(), &LintThreadManager::slotStartLintManager);
    // Get linter data from MainWindow
    connect(m_lintThreadManager.get(), &LintThreadManager::signalGetLinterData, m_parent, &MainWindow::slotGetLinterData);
    // Send linter data to Lint thread manager
    connect(m_parent, &MainWindow::signalSetLinterData, m_lintThreadManager.get(), &LintThreadManager::slotSetLinterData);
    // Tell MainWindow we are done
    connect(this, &ProgressWindow::signalLintFinished, m_parent, &MainWindow::slotLintFinished);

    connect(this, &ProgressWindow::signalLintComplete, m_parent, &MainWindow::slotLintComplete);

    ui->lintGroupBox->setTitle(title);

    emit signalStartLintManager();
}

void ProgressWindow::slotUpdateProgress(int value) noexcept
{
    m_currentProgress += value;
    qDebug() << "Progress now: (" << m_currentProgress << "/" << m_progressMax << ")";
    ui->lintProgressBar->setValue(m_currentProgress);
}

void ProgressWindow::slotUpdateProgressMax(int value) noexcept
{
    qDebug() << "Progress max received maximum: " << value;
    // Accumulate progress
    m_progressMax += value;
    ui->lintProgressBar->setMaximum(m_progressMax);
}

void ProgressWindow::slotUpdateProcessedFiles(int processedFiles) noexcept
{
   m_currentFileProgress += processedFiles;
   ui->filesProcessed->setText(QString::number(m_currentFileProgress)+ "/" + QString::number(m_progressMax));
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
    ui->timeElapsed->setText(QDateTime::fromTime_t(m_elapsedTime++).toUTC().toString("hh:mm:ss"));
    if (m_eta >= 0)
    {
        ui->eta->setText(QDateTime::fromTime_t(m_eta--).toUTC().toString("hh:mm:ss"));
    }
}

void ProgressWindow::slotLintFinished(const LintResponse& lintResponse) noexcept
{
    emit signalLintFinished(lintResponse);
}

void ProgressWindow::slotLintComplete() noexcept
{
    emit signalLintComplete();
    close();
}

ProgressWindow::~ProgressWindow()
{
    if (m_aborted == true)
    {
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION);
    }
    else
    {
        m_parent->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_windowTitle);
    }
    delete ui;
    m_workerThread->quit();
    Q_ASSERT(m_workerThread->wait());
    qDebug() << "ProgressWindow destroyed";
}

void ProgressWindow::on_lintCancel_clicked()
{
    m_aborted = true;
    emit signalAbortLint();
}
