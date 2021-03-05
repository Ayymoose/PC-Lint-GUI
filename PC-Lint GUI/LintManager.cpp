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

#include "LintManager.h"
#include <QDebug>
#include <thread>

namespace PCLint
{

LintManager::LintManager(QObject *parent):
    m_completedLints(0),
    m_parent(parent)
{

    QObject::connect(this, &LintManager::signalLintFinished, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintFinished);
    QObject::connect(this, &LintManager::signalLintComplete, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintComplete);
    QObject::connect(static_cast<ProgressWindow*>(m_parent), &ProgressWindow::signalAbortLint,this, &LintManager::slotAbortLint);
}

void LintManager::slotGetLintData(const LintData& lintData) noexcept
{
    qDebug() << "ProgressWindow sends Lint data to LintManager";

    Q_ASSERT(lintData.lintFiles.size());
    Q_ASSERT(lintData.lintOptionFile.size());
    Q_ASSERT(lintData.linterExecutable.size());

    qDebug() << "Number of lint files: " << lintData.lintFiles.size();
    qDebug() << "Lint option file: " << lintData.lintOptionFile;
    qDebug() << "Lint executable: " << lintData.linterExecutable;
    m_lintData = lintData;
}

void LintManager::slotStartLint() noexcept
{
    qDebug() << "ProgressWindow wants LintManager to start lint";

    Q_ASSERT(m_lintData.lintFiles.size());
    Q_ASSERT(m_lintData.lintOptionFile.size());
    Q_ASSERT(m_lintData.linterExecutable.size());

    // Because the maximum data we can supply on command line to a QProcess is 8192 characters on Windows
    // We will need to split it up in chunks

    // Assume PC-Lint management data doesn't consume more than 512 characters
    // While we still have files to add, add as many files as possible until it reaches 8192 characters
    // Give to one lint object
    // Repeat until no files left then start

    QSet<QSet<QString>> lintFilePerThread;
    QSet<QString> lintFiles;
    int length = MAX_LINT_PATH;

    for (const auto& file : m_lintData.lintFiles)
    {
        if (length + file.size() < MAX_PROCESS_CHARACTERS)
        {
            lintFiles.insert(file);
            length += file.size();
        }
        else
        {
            lintFilePerThread.insert(lintFiles);
            length = MAX_LINT_PATH;
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
    std::for_each(lintFilePerThread.begin(), lintFilePerThread.end(),[&size](const auto& list)
    {
        size += list.size();
    });
    Q_ASSERT(size == m_lintData.lintFiles.size());

    m_completedLints = 0;
    m_lints.clear();

    for (const auto& lintFileList : lintFilePerThread)
    {
        // Create new lint and hand it the data
        auto lint = std::make_unique<Lint>();

        QObject::connect(lint.get(), &Lint::signalLintComplete, this, &LintManager::slotLintComplete);
        QObject::connect(lint.get(), &Lint::signalUpdateProgress, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgress);
        QObject::connect(lint.get(), &Lint::signalUpdateProgressMax, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgressMax);
        QObject::connect(lint.get(), &Lint::signalUpdateProcessedFiles, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProcessedFiles);


        // Asynchronous start
        LintData perLintData = m_lintData;
        perLintData.lintFiles = lintFileList;

        // Go!
        lint->setLintData(perLintData);
        lint->asyncLint();

        m_lints.emplace_back(std::move(lint));
    }

    qDebug() << "Running" << QString::number(m_lints.size()) << " instances";
}

void LintManager::slotLintComplete(const LintResponse& lintResponse) noexcept
{
    qDebug() << "Lint finished with:" << lintResponse.status;
    qDebug() << "Number of lint messages:" << lintResponse.lintMessages.size();
    qDebug() << "Number of lint errors:" << lintResponse.numberOfErrors;
    qDebug() << "Number of lint warnings:" << lintResponse.numberOfWarnings;
    qDebug() << "Number of lint information:" << lintResponse.numberOfInfo;

    emit signalLintFinished(lintResponse);
    m_completedLints++;

    if (m_completedLints == m_lints.size())
    {
        emit signalLintComplete();
    }
}

void LintManager::joinAll() const noexcept
{
    for (auto const& thread : m_lintThreads)
     {
         thread->quit();
     }
     std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](auto const& thread)
     {
         auto waitComplete = thread->wait(MAX_THREAD_WAIT);
         Q_ASSERT(waitComplete);
     });
}

LintManager::~LintManager()
{
    // TODO: Abort lint
}

void LintManager::slotLintFinished(const LintResponse& lintResponse) noexcept
{

    qDebug() << "Lint finished with:" << lintResponse.status;
    qDebug() << "Number of lint messages:" << lintResponse.lintMessages.size();
    qDebug() << "Number of lint errors:" << lintResponse.numberOfErrors;
    qDebug() << "Number of lint warnings:" << lintResponse.numberOfWarnings;
    qDebug() << "Number of lint information:" << lintResponse.numberOfInfo;

    emit signalLintFinished(lintResponse);
    m_completedLints++;

    if (m_completedLints == m_lintThreads.size())
    {
        // Join all
        joinAll();
        emit signalLintComplete();
    }
}

void LintManager::slotAbortLint() noexcept
{
    qDebug() << "Attempting to abort lint";
    int threads = 1;
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[](const auto& thread)
    {
        thread->requestInterruption();
        thread->quit();
    });
    std::for_each(m_lintThreads.begin(), m_lintThreads.end(),[this,&threads](const auto& thread)
    {
        auto waitComplete = thread->wait(MAX_THREAD_WAIT);
        Q_ASSERT(waitComplete);
        qDebug() << threads++ << '/' << m_lintThreads.size() << " finished";
    });
    qDebug() << "Lint was aborted";
    emit signalLintComplete();
}

};
