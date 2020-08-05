#ifndef LINTTHREADMANAGER_H
#define LINTTHREADMANAGER_H

#include <QObject>
#include <QThread>

#include <vector>
#include <memory>
#include "Linter.h"

class LintThreadManager : public QObject
{
    Q_OBJECT
public:
    explicit LintThreadManager(QObject *parent = nullptr);
    ~LintThreadManager();
signals:
    void signalGetLinterData();
    void signalSetLinterData(const LintData& lintData);
    void signalStartLint(bool start);
    void signalLintFinished(const LintResponse& lintResponse);
    void signalLintComplete();
public slots:
    void slotSetLinterData(const LintData& lintData);
    void slotStartLintManager();
    void slotAbortLint();
private slots:
    void slotLintFinished(const LintResponse& lintResponse);
private:
    void startLint();
    std::vector<std::unique_ptr<QThread>> m_lintThreads;
    std::vector<std::unique_ptr<Linter>> m_lintPointers;
    LintData m_lintData;
    unsigned int m_completedLints;
    QObject* m_parent;
};

#endif // LINTTHREADMANAGER_H
