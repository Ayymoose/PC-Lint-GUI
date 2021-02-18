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

#pragma once

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
    void slotSetLinterData(const LintData& lintData) noexcept;
    void slotStartLintManager() noexcept;
    void slotAbortLint() noexcept;
private slots:
    void slotLintFinished(const LintResponse& lintResponse) noexcept;
private:
    void startLint() noexcept;
    std::vector<std::unique_ptr<QThread>> m_lintThreads;
    std::vector<std::unique_ptr<Linter>> m_lintPointers;
    LintData m_lintData;
    unsigned int m_completedLints;
    QObject* m_parent;

    void joinAll() const noexcept;
};
