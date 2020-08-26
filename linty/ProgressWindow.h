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
    void slotLintFinished(const LintResponse& lintResponse) noexcept;
    void slotLintComplete() noexcept;

private slots:
    void on_lintCancel_clicked();
    void slotUpdateTime() noexcept;

signals:
    void signalLintComplete();
    void signalLintFinished(const LintResponse& lintResponse);
    void signalAbortLint();
    void signalStartLintManager();
private:
    Ui::ProgressWindow *ui;
    int m_elapsedTime;
    int m_eta;
    int m_progressMax;
    int m_currentProgress;
    int m_fileProgressMax;
    int m_currentFileProgress;
    bool m_aborted;
    std::unique_ptr<QTimer> m_timer;
    QString m_windowTitle;
    std::unique_ptr<LintThreadManager> m_lintThreadManager;
    std::unique_ptr<QThread> m_workerThread;
    MainWindow* m_parent;
};

#endif // PROGRESSWINDOW_H
