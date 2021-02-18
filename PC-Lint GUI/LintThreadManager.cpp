// PC-Lint GUI
// Copyright (C) 2021  Ayymooose

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

void LintThreadManager::joinAll() const noexcept
{
    for (auto const& thread : m_lintThreads)
     {
         thread->quit();
     }
     std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](auto const& thread)
     {
         Q_ASSERT(thread->wait());
     });
}

LintThreadManager::~LintThreadManager()
{
    // Join all
    joinAll();
    qDebug() << "LintThreadManager destroyed";
}

void LintThreadManager::slotSetLinterData(const LintData& lintData) noexcept
{
    Q_ASSERT(lintData.lintFiles.size());
    Q_ASSERT(lintData.lintOptionFile.size());
    Q_ASSERT(lintData.linterExecutable.size());

    qDebug() << "[" << QThread::currentThreadId() << "]" << "Number of lint files: " << lintData.lintFiles.size();
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Lint option file: " << lintData.lintOptionFile;
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Lint executable: " << lintData.linterExecutable;
    m_lintData = lintData;
    emit startLint();
}

void LintThreadManager::slotStartLintManager() noexcept
{
    qDebug() << "[" << QThread::currentThreadId() << "]" << "Starting Lint Manager";
    emit signalGetLinterData();
}

void LintThreadManager::startLint() noexcept
{
    // Because the maximum we can write to a process is 8192 characters on Windows
    // We will need to split it between threads

    // Assume linter doesn't consume more than 512 characters (use assertion in linter)
    // While we still have files to add, add as many files as possible until it reaches 8192 characters
    // Give to one thread
    // Repeat until no files left then start all threads

    QSet<QSet<QString>> lintFilePerThread;
    QSet<QString> lintFiles;
    int length = Lint::MAX_LINT_PATH;

    for (const QString& file : m_lintData.lintFiles)
    {
        if (length + file.size() < Lint::MAX_PROCESS_CHARACTERS)
        {
            lintFiles.insert(file);
            length += file.size();
        }
        else
        {
            lintFilePerThread.insert(lintFiles);
            length = Lint::MAX_LINT_PATH;
            lintFiles.clear();
            lintFiles.insert(file);
        }
    }
    if (lintFiles.size())
    {
        lintFilePerThread.insert(lintFiles);
    }

    // Sanity check
    int size = 0;
    std::for_each(lintFilePerThread.begin(), lintFilePerThread.end(),[&size](const QSet<QString>& list){ size += list.size(); });
    Q_ASSERT(size == m_lintData.lintFiles.size());

    m_lintPointers.clear();
    m_lintThreads.clear();

    int count = 0;
    for (const QSet<QString>& lintFileList : lintFilePerThread)
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

void LintThreadManager::slotLintFinished(const LintResponse& lintResponse) noexcept
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
        joinAll();
        emit signalLintComplete();
    }
}

void LintThreadManager::slotAbortLint() noexcept
{
    qDebug() << "Attempting to abort lint...";
    int threads = 1;
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](const auto& thread)
    {
        thread->requestInterruption();
        thread->quit();
    });
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[this,&threads](const auto& thread)
    {
        Q_ASSERT(thread->wait());
        qDebug() << threads++ << "/" << m_lintThreads.size() << " finished";
    });
    qDebug() << "Lint aborted!";
    emit signalLintComplete();
}
