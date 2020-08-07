#include "LintThreadManager.h"
#include <QDebug>
#include <thread>
#include "ProgressWindow.h"
#include "Linter.h"

LintThreadManager::LintThreadManager(QObject *parent)
{
    m_parent = parent;
    m_completedLints = 0;

    connect(this, &LintThreadManager::signalLintFinished, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintFinished);
    connect(this, &LintThreadManager::signalLintComplete, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintComplete);
    connect(dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::signalAbortLint,this, &LintThreadManager::slotAbortLint);

}

LintThreadManager::~LintThreadManager()
{
    for (auto const& thread : m_lintThreads)
    {
        thread->quit();
        Q_ASSERT(thread->wait());
    }
    qDebug() << "LintThreadManager destroyed";
}

void LintThreadManager::slotSetLinterData(const LintData& lintData)
{
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Number of lint files: " << lintData.lintFiles.size();
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Lint option file: " << lintData.lintOptionFile;
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Lint executable: " << lintData.linterExecutable;
    m_lintData = lintData;
    emit startLint();
}

void LintThreadManager::slotStartLintManager()
{
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Starting Lint Manager";
    emit signalGetLinterData();
}

void LintThreadManager::startLint()
{
    // Because the maximum we can write to a process is 8192 characters on Windows
    // We will need to split it between threads

    // Assume linter doesn't consume more than 512 characters (use assertion in linter)
    // While we still have files to add, add as many files as possible until it reaches 8192 characters
    // Give to one thread
    // Repeat until no files left then start all threads

    QList<QList<QString>> lintFilePerThread;
    QList<QString> lintFiles;
    int length = 512;

    for (const QString& file : m_lintData.lintFiles)
    {
        if (length + file.size() < MAX_PROCESS_CHARACTERS)
        {
            lintFiles.append(file);
            length += file.size();
        }
        else
        {
            lintFilePerThread.append(lintFiles);
            length = 512;
            lintFiles.clear();
            lintFiles.append(file);
        }
    }
    if (lintFiles.size())
    {
        lintFilePerThread.append(lintFiles);
    }

    // Sanity check
    int size = 0;
    std::for_each(lintFilePerThread.begin(), lintFilePerThread.end(),[&size](const QList<QString>& list){ size += list.size(); });
    Q_ASSERT(size == m_lintData.lintFiles.size());

    m_lintPointers.clear();
    m_lintThreads.clear();

    int count = 0;
    for (const QList<QString>& lintFileList : lintFilePerThread)
    {

        // Create a new thread
        auto lintThread = std::make_unique<QThread>(this);
        // Create a new linter
        auto linter = std::make_unique<Linter>();
        // Assign the data
        LintData perLintData = m_lintData;
        perLintData.lintFiles = lintFileList;

        connect(this, &LintThreadManager::signalSetLinterData, linter.get(), &Linter::slotGetLinterData);
        connect(this, &LintThreadManager::signalStartLint, linter.get(), &Linter::slotStartLint);
        connect(linter.get(), &Linter::signalLintFinished, this, &LintThreadManager::slotLintFinished);


        connect(linter.get(), &Linter::signalUpdateProgress, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgress);
        connect(linter.get(), &Linter::signalUpdateProgressMax, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgressMax);
        connect(linter.get(), &Linter::signalUpdateProcessedFiles, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProcessedFiles);
        connect(linter.get(), &Linter::signalUpdateETA, dynamic_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateETA);

        // Now move to thread
        linter->moveToThread(lintThread.get());
        lintThread->start();

        m_lintThreads.emplace_back(std::move(lintThread));
        m_lintPointers.emplace_back(std::move(linter));

        emit signalSetLinterData(perLintData);
        emit signalStartLint(true);

        disconnect(this, &LintThreadManager::signalSetLinterData, m_lintPointers[count].get(), &Linter::slotGetLinterData);
        disconnect(this, &LintThreadManager::signalStartLint, m_lintPointers[count].get(), &Linter::slotStartLint);

        count++;
    }
}

void LintThreadManager::slotLintFinished(const LintResponse& lintResponse)
{
    const QString debugString = "[LintThreadManager::slotLintFinished]";

    qDebug() << debugString << "Lint finished with " << lintResponse.status;
    qDebug() << debugString << "Number of lint messages: " << lintResponse.lintMessages.size();
    qDebug() << debugString << "Number of lint errors: " << lintResponse.numberOfErrors;
    qDebug() << debugString << "Number of lint warnings: " << lintResponse.numberOfWarnings;
    qDebug() << debugString << "Number of lint information: " << lintResponse.numberOfInfo;

    emit signalLintFinished(lintResponse);
    m_completedLints++;

    if (m_completedLints == m_lintThreads.size())
    {
        // Join all
        std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](auto& thread)
        {
            thread->quit();
            Q_ASSERT(thread->wait());
        });

        emit signalLintComplete();
    }
}

void LintThreadManager::slotAbortLint()
{
    qDebug() << "Attempting to abort lint...";
    int threads = 1;
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](auto& thread)
    {
        thread->requestInterruption();
        thread->quit();
    });
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[this,&threads](auto& thread)
    {
        Q_ASSERT(thread->wait());
        qDebug() << threads++ << "/" << m_lintThreads.size() << " finished";
    });
    qDebug() << "Lint aborted!";
}
