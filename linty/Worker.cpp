#include "Worker.h"
#include <QDebug>
#include <windows.h>
#include "Linter.h"

void Worker::slotStartLint(MainWindow* pMainWindow)
{
    m_pMainWindow = pMainWindow;
    Linter* linter = m_pMainWindow->getLinter();
    LINTER_STATUS status = linter->lint();

    // Worker thread does the linting and returns the status
    // If problem then reports back to MainWindow

    emit signalLintFinished(status);
}

void Worker::slotParseData()
{
    // There were no errors when starting lint
    // So now fill the table
    emit signalUpdateLintTable();

    //Nothing left to do for worker,
    emit signalFinished();
}

Worker::~Worker()
{
    qDebug() << "Worker has died";
}
