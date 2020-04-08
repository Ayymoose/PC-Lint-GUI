#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include <QDialog>
#include <QSet>
#include "Linter.h"
#include "MainWindow.h"
#include "Worker.h"
#include <QTime>

class MainWindow;
class Worker;

namespace Ui
{
class ProgressWindow;
}

class ProgressWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressWindow(QWidget *parent = nullptr);
    ~ProgressWindow();
    void lintProcess();


public slots:
    void slotUpdateProgress(int value);
    void slotUpdateProgressMax(int value);
    void slotLintFinished(LINTER_STATUS status);
    void slotUpdateStatus(QString status);
    void slotLintComplete();
    void slotUpdateTime();
    void slotUpdateProgressTitle(QString title);

private slots:
    void on_lintCancel_clicked();

signals:
    void signalStartLint(MainWindow* pMainWindow);
    void signalParseData();


private:
    Ui::ProgressWindow *ui;
    MainWindow* m_mainWindow;
    QString m_lintStatus;
    int m_elapsedTime;
    int m_eta;
    QTimer* m_timer;
    QString m_windowTitle;
    QThread* m_workerThread;
    Worker* m_worker;

};

#endif // PROGRESSWINDOW_H
