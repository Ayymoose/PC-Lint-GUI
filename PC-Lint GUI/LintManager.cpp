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
#include "ProgressWindow.h"
#include "Lint.h"

namespace PCLint
{

LintManager::LintManager(QObject *parent): m_completedLints(0), m_parent(parent)
{
    QObject::connect(this, &LintManager::signalLintFinished, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintFinished);
    QObject::connect(this, &LintManager::signalLintComplete, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotLintComplete);
    QObject::connect(static_cast<ProgressWindow*>(m_parent), &ProgressWindow::signalAbortLint,this, &LintManager::slotAbortLint);
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
    // Join all
    joinAll();
    qDebug() << "LintManager destroyed";
}

void LintManager::slotSetLinterData(const LintData& lintData) noexcept
{
    Q_ASSERT(lintData.lintFiles.size());
    Q_ASSERT(lintData.lintOptionFile.size());
    Q_ASSERT(lintData.linterExecutable.size());

    qDebug() << '[' << QThread::currentThreadId() << "] Number of lint files: " << lintData.lintFiles.size();
    qDebug() << '[' << QThread::currentThreadId() << "] Lint option file: " << lintData.lintOptionFile;
    qDebug() << '[' << QThread::currentThreadId() << "] Lint executable: " << lintData.linterExecutable;
    m_lintData = lintData;
    emit startLint();
}

void LintManager::slotStartLintManager() noexcept
{
    qDebug() << '[' << QThread::currentThreadId() << "] Starting Lint Manager";
    emit signalGetLinterData();
}

void LintManager::startLint() noexcept
{
    // Because the maximum we can write to a process is 8192 characters on Windows
    // We will need to split it between threads

    // Assume linter doesn't consume more than 512 characters (use assertion in linter)
    // While we still have files to add, add as many files as possible until it reaches 8192 characters
    // Give to one thread
    // Repeat until no files left then start all threads



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
    std::for_each(lintFilePerThread.begin(), lintFilePerThread.end(),[&size](const QSet<QString>& list){ size += list.size(); });
    Q_ASSERT(size == m_lintData.lintFiles.size());

    m_lintPointers.clear();
    m_lintThreads.clear();

    // TODO: There is a bug in here that is causing QProcess to crash randomly

    int count = 0;
    for (const auto& lintFileList : lintFilePerThread)
    {

        // Create a new thread
    /*    auto lintThread = std::make_unique<QThread>(this);
        // Create a new linter
        auto linter = std::make_unique<Linter>();*/

        m_lintThreads.emplace_back(std::make_unique<QThread>(this));
        m_lintPointers.emplace_back(std::make_unique<Lint>());


        m_lintPointers[count]->moveToThread(m_lintThreads[count].get());
        m_lintThreads[count]->start();

        // Now move to thread
        /*linter->moveToThread(lintThread.get());
        lintThread->start();*/


       // QObject::connect(this, &LintManager::signalSetLinterData, m_lintPointers[count].get(), &slotGetLintData);
       // QObject::connect(this, &LintManager::signalStartLint, m_lintPointers[count].get(), &slotStartLint);
       // QObject::connect(m_lintPointers[count].get(), &signalLintFinished, this, &LintManager::slotLintFinished);
/*

        QObject::connect(m_lintPointers[count].get(), &Linter::signalUpdateProgress, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgress);
        QObject::connect(m_lintPointers[count].get(), &Linter::signalUpdateProgressMax, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProgressMax);
        QObject::connect(m_lintPointers[count].get(), &Linter::signalUpdateProcessedFiles, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateProcessedFiles);
        QObject::connect(m_lintPointers[count].get(), &Linter::signalUpdateETA, static_cast<ProgressWindow*>(m_parent), &ProgressWindow::slotUpdateETA);

*/
        // Assign the data
        LintData perLintData = m_lintData;
        perLintData.lintFiles = lintFileList;

        emit signalSetLinterData(perLintData);
        emit signalStartLint(true);



        //QObject::disconnect(this, &LintManager::signalSetLinterData, m_lintPointers[count].get(), &Linter::slotGetLinterData);
        //QObject::disconnect(this, &LintManager::signalStartLint, m_lintPointers[count].get(), &Linter::slotStartLint);

        count++;
    }

    qDebug() << count << "threads created";
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
