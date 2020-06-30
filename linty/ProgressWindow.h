#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H


#include "Linter.h"
#include "MainWindow.h"

#include <QTime>
#include <QDialog>
#include <QSet>

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

    void setLintData(const LintData& lintData);


public slots:
    void slotUpdateProgress(int value);
    void slotUpdateProgressMax(int value);
    void slotLintFinished(const LintResponse& lintResponse);
    void slotLintComplete();

    void slotUpdateETA(int eta);

    void slotUpdateProgressTitle(QString title);
    void slotUpdateProcessedFiles(int processedFiles, int processedFilesMax);

    // Called by MainWindow
    void slotStartLint();

private slots:
    void on_lintCancel_clicked();
    void slotUpdateTime();

signals:
    void signalParseData();
    void signalLintError(LINTER_STATUS status);
    void signalStartLint(bool start);
    void signalSetLinterData(const LintData& lintData);
    void signalLintFinished(const LintResponse& lintResponse);

private:
    Ui::ProgressWindow *ui;
    MainWindow* m_mainWindow;
    QString m_lintStatus;
    int m_elapsedTime;
    int m_eta;
    std::unique_ptr<QTimer> m_timer;
    QString m_windowTitle;
    std::unique_ptr<QThread> m_workerThread;
    LintData m_lintData;
    Linter m_linter;
};

#endif // PROGRESSWINDOW_H
