#include "ProgressWindow.h"
#include "ui_ProgressWindow.h"
#include "Linter.h"
#include "Jenkins.h"

#include <QDebug>
#include <QTimer>
#include <QThread>

ProgressWindow::ProgressWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressWindow)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_elapsedTime = 0;
    m_eta = 5;
    ui->setupUi(this);
    ui->lintProgressBar->setValue(0);
    m_timer = std::make_unique<QTimer>();

    connect(m_timer.get(), &QTimer::timeout, this, &ProgressWindow::slotUpdateTime);
    m_timer->start(1000);

    m_workerThread = std::make_unique<QThread>(this);
    m_linter.moveToThread(m_workerThread.get());
    m_workerThread->start();

    connect(&m_linter, &Linter::signalLintFinished, this, &ProgressWindow::slotLintFinished);
    connect(this, &ProgressWindow::signalStartLint, &m_linter, &Linter::slotStartLint);
    connect(this, &ProgressWindow::signalSetLinterData, &m_linter, &Linter::slotGetLinterData);

    connect(&m_linter, &Linter::signalUpdateProgress, this, &ProgressWindow::slotUpdateProgress);
    connect(&m_linter, &Linter::signalUpdateProgressMax, this, &ProgressWindow::slotUpdateProgressMax);
    connect(&m_linter, &Linter::signalUpdateProcessedFiles, this, &ProgressWindow::slotUpdateProcessedFiles);
    connect(&m_linter, &Linter::signalUpdateETA, this, &ProgressWindow::slotUpdateETA);


    connect(this, &ProgressWindow::signalLintFinished, dynamic_cast<MainWindow*>(parent), &MainWindow::slotLintFinished);

    QTimer::singleShot(250,this,&ProgressWindow::slotStartLint);
}

void ProgressWindow::slotUpdateProgress(int value)
{
    ui->lintProgressBar->setValue(value);
}

void ProgressWindow::slotUpdateProgressMax(int value)
{
    ui->lintProgressBar->setMaximum(value);
}

void ProgressWindow::slotUpdateProcessedFiles(int processedFiles, int processedFilesMax)
{
   ui->filesProcessed->setText(QString::number(processedFiles)+ "/" + QString::number(processedFilesMax));
}

void ProgressWindow::slotUpdateETA(int eta)
{
    m_eta = eta;
}

void ProgressWindow::slotUpdateTime()
{
    ui->timeElapsed->setText(QDateTime::fromTime_t(m_elapsedTime++).toUTC().toString("hh:mm:ss"));
    if (m_eta >= 0)
    {
        ui->eta->setText(QDateTime::fromTime_t(m_eta--).toUTC().toString("hh:mm:ss"));
    }

}

void ProgressWindow::slotLintComplete()
{
    m_mainWindow->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_windowTitle);
    close();
}

void ProgressWindow::slotLintFinished(const LintResponse& lintResponse)
{
    // Send to MainWindow
    emit signalLintFinished(lintResponse);
    close();
}

void ProgressWindow::slotUpdateProgressTitle(QString title)
{
    ui->lintGroupBox->setTitle(title);
    m_windowTitle = title;
}

ProgressWindow::~ProgressWindow()
{
    delete ui;
    m_workerThread->quit();
    Q_ASSERT(m_workerThread->wait());
}

// Kick off the lint thread
void ProgressWindow::slotStartLint()
{
    emit signalStartLint(true);
}

// Send the lint data to the Linter object
void ProgressWindow::setLintData(const LintData& lintData)
{
    emit signalSetLinterData(lintData);
}

void ProgressWindow::on_lintCancel_clicked()
{
    m_workerThread->requestInterruption();
}
