#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include "Linter.h"
#include "MainWindow.h"
#include "LintThreadManager.h"

#include <QTime>
#include <QDialog>
#include <QSet>
#include <vector>
#include <memory>

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
    explicit ProgressWindow(QWidget *parent = nullptr, const QString& title = "");
    ~ProgressWindow();

public slots:
    void slotUpdateProgress(int value) noexcept;
    void slotUpdateProgressMax(int value) noexcept;
    void slotUpdateETA(int eta) noexcept;
    void slotUpdateProgressTitle(QString title) noexcept;
    void slotUpdateProcessedFiles(int processedFiles) noexcept;
    void slotLintFinished(const LintResponse& lintResponse) noexcept;
    void slotLintComplete() noexcept;

private slots:
    void on_lintCancel_clicked();
    void slotUpdateTime() noexcept;

signals:
    void signalStartLint(bool start);
    void signalSetLinterData(const LintData& lintData);
    void signalLintComplete();
    void signalLintFinished(const LintResponse& lintResponse);
    void signalAbortLint();
    void signalStartLintManager();
private:
    Ui::ProgressWindow *ui;
    MainWindow* m_mainWindow;
    QString m_lintStatus;
    int m_elapsedTime;
    int m_eta;
    int m_progressMax;
    int m_currentProgress;
    int m_fileProgressMax;
    int m_currentFileProgress;
    std::unique_ptr<QTimer> m_timer;
    QString m_windowTitle;
    std::unique_ptr<LintThreadManager> m_lintThreadManager;
    std::unique_ptr<QThread> m_workerThread;
    LintData m_lintData;
};

#endif // PROGRESSWINDOW_H
