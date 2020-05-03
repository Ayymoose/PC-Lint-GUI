#include "ProgressWindow.h"
#include "ui_ProgressWindow.h"
#include <QDebug>
#include "Worker.h"
#include "Linter.h"
#include "Jenkins.h"
#include <QThread>

ProgressWindow::ProgressWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressWindow)
{
    m_mainWindow = dynamic_cast<MainWindow*>(parent);
    Q_ASSERT(m_mainWindow != nullptr);
    m_elapsedTime = 0;
    m_eta = 5;
    ui->setupUi(this);
    ui->lintProgressBar->setValue(0);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &ProgressWindow::slotUpdateTime);

}

void ProgressWindow::slotUpdateProgress(int value)
{
    ui->lintProgressBar->setValue(value);
}


void ProgressWindow::slotUpdateProgressMax(int value)
{
    ui->lintProgressBar->setMaximum(value);
}

void ProgressWindow::slotUpdateStatus(QString status)
{
    ui->labelLintStatus->setText(status);
}

void ProgressWindow::lintProcess()
{
    m_timer->start(1000);

    m_workerThread = new QThread(this);
    m_worker = new Worker;
    m_worker->moveToThread(m_workerThread);
    connect(this, &ProgressWindow::signalStartLint, m_worker, &Worker::slotStartLint);

    connect( m_worker, &Worker::signalFinished, m_worker, [=]
    {
         m_worker->deleteLater();
         m_workerThread->quit();
         m_workerThread = Q_NULLPTR;
    });

    connect(m_worker, &Worker::signalLintFinished, this, &ProgressWindow::slotLintFinished);
    connect(this, &ProgressWindow::signalParseData, m_worker, &Worker::slotParseData);

    connect(this, &ProgressWindow::signalLintError, m_mainWindow, &MainWindow::slotLintError);

    connect(m_mainWindow, &MainWindow::signalUpdateProgressTitle, this, &ProgressWindow::slotUpdateProgressTitle);

    // Update the lint table
    connect(m_worker, &Worker::signalUpdateLintTable, m_mainWindow, &MainWindow::slotUpdateLintTable);

    Linter* linter = m_mainWindow->getLinter();

    connect(linter, &Linter::signalUpdateProgress, this, &ProgressWindow::slotUpdateProgress);
    connect(linter, &Linter::signalUpdateProgressMax, this, &ProgressWindow::slotUpdateProgressMax);
    connect(linter, &Linter::signalUpdateStatus, this, &ProgressWindow::slotUpdateStatus);

    connect(m_mainWindow, &MainWindow::signalUpdateProgress, this, &ProgressWindow::slotUpdateProgress);
    connect(m_mainWindow, &MainWindow::signalUpdateProgressMax, this, &ProgressWindow::slotUpdateProgressMax);
    connect(m_mainWindow, &MainWindow::signalUpdateStatus, this, &ProgressWindow::slotUpdateStatus);
    connect(m_mainWindow, &MainWindow::signalLintComplete, this, &ProgressWindow::slotLintComplete);

    m_workerThread->start();
    emit signalStartLint(m_mainWindow);

}

void ProgressWindow::slotUpdateTime()
{
    ui->labelTimeElapsed->setText(QDateTime::fromTime_t(m_elapsedTime++).toUTC().toString("hh:mm:ss"));
    if (m_eta >= 0)
    {
        ui->labelETA->setText(QDateTime::fromTime_t(m_eta--).toUTC().toString("hh:mm:ss"));
    }

}

void ProgressWindow::slotLintComplete()
{
    m_mainWindow->setWindowTitle(APPLICATION_NAME " " BUILD_VERSION " - " + m_windowTitle);
    close();
}

void ProgressWindow::slotLintFinished(LINTER_STATUS status)
{
    if (status == LINTER_OK)
    {
        // Start parsing the data
        emit signalParseData();
    }
    else
    {
        // Some error occured so let MainWindow handle it
        emit signalLintError(status);
        close();
    }
}

void ProgressWindow::slotUpdateProgressTitle(QString title)
{
    ui->lintGroupBox->setTitle(title);
    m_windowTitle = title;
}

ProgressWindow::~ProgressWindow()
{
    delete ui;
    delete m_timer;
    if (m_workerThread)
    {
        m_workerThread->quit();
        m_workerThread->wait();
    }

}

void ProgressWindow::on_lintCancel_clicked()
{
    close();
}
