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

LintManager::LintManager(QObject*)
{

    // Currently only supporting PC-Lint Plus
    QObject::connect(&m_lint, &Lint::signalUpdateProgress, this, &LintManager::slotUpdateProgress);
    QObject::connect(&m_lint, &Lint::signalUpdateProgressMax, this, &LintManager::slotUpdateProgressMax);
    QObject::connect(&m_lint, &Lint::signalUpdateProcessedFiles, this, &LintManager::slotUpdateProcessedFiles);

    // Lint passes processed chunk to ProgressWindow
    QObject::connect(&m_lint, &Lint::signalProcessLintMessages, this, &LintManager::slotProcessLintMessages);


    QObject::connect(&m_lint, &Lint::signalLintComplete, this, &LintManager::slotLintComplete);
    QObject::connect(this, &LintManager::signalAbortLint, &m_lint, &Lint::slotAbortLint);

}

void LintManager::slotUpdateProgress(int value) noexcept
{
    emit signalUpdateProgress(value);
}

void LintManager::slotUpdateProgressMax(int value) noexcept
{
    emit signalUpdateProgressMax(value);
}

void LintManager::slotUpdateProcessedFiles(int processedFiles) noexcept
{
    emit signalUpdateProcessedFiles(processedFiles);
}

void LintManager::slotProcessLintMessages(const LintResponse& lintResponse) noexcept
{
    emit signalProcessLintMessages(lintResponse);
}

void LintManager::slotGetLintData(const LintData& lintData) noexcept
{
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
    Q_ASSERT(m_lintData.lintFiles.size());
    Q_ASSERT(m_lintData.lintOptionFile.size());
    Q_ASSERT(m_lintData.linterExecutable.size());

    // Asynchronous start
    m_lint.setLintData(m_lintData);
    m_lint.lint();

}

void LintManager::slotLintComplete(const LintStatus& lintStatus, const QString& errorMessage) noexcept
{
    emit signalLintComplete(lintStatus, errorMessage);
}

LintManager::~LintManager()
{
    // TODO: Abort lint
}

void LintManager::slotLintFinished(const LintResponse& ) noexcept
{


}

void LintManager::slotAbortLint() noexcept
{
    emit signalAbortLint();
}

};
