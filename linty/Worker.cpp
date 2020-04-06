#include "Worker.h"
#include <QDebug>
#include <windows.h>
#include "Linter.h"

void Worker::slotStartLint(MainWindow* pMainWindow)
{
    m_pMainWindow = pMainWindow;
    Linter* linter = m_pMainWindow->getLinter();
    LINTER_STATUS status = linter->lint(m_lintMessages);
    emit signalLintFinished(status);
}

void Worker::slotParseData()
{
    // There were no errors when starting lint
    // So now fill the table
    emit signalUpdateLintTable(m_lintMessages);
    emit signalFinished();
}

Worker::~Worker()
{
    qDebug() << "Worker has died";
}
