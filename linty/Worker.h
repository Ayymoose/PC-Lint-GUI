#ifndef WORKER_H
#define WORKER_H

#include <QTimer>
#include <QObject>
#include "MainWindow.h"

class MainWindow;

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject* parent = nullptr) : QObject(parent)
    {
    }
    ~Worker();
public slots:
    void slotStartLint(MainWindow* pMainWindow);
    void slotParseData();
signals:
    void signalLintFinished(LINTER_STATUS status);
    void signalUpdateLintTable();
    void signalFinished();
private:
    QSet<lintMessage> m_lintMessages;
    MainWindow* m_pMainWindow;

};

#endif // WORKER_H
