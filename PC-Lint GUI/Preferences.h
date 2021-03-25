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

#include "Lint.h"
#include <QDialog>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QThread>
#include <QProcess>

#include <QSettings>

namespace PCLint
{
const QString SETTINGS_APPLICATION_NAME = "PC-Lint GUI";
const QString SETTINGS_GROUP_NAME = "Settings";
const QString SETTINGS_MAX_THREADS = "MaxThreads";
const QString SETTINGS_LINT_EXECUTABLE_PATH = "LintExecutablePath";
const QString SETTINGS_LINT_FILE_PATH = "LintFilePath";
const QString SETTINGS_LAST_DIRECTORY = "LastDirectory";
};

namespace Ui
{
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
    QString getLintExecutablePath() const noexcept;
    QString getLintFilePath() const noexcept;
    int getLintHardwareThreads() const noexcept;
    static QString m_lastDirectory;
    //void reject() override;

signals:
    void signalLintVersion(const PCLint::Version& version);

private slots:
    void on_lintPathFileOpen_clicked();
    void on_lintFileFileOpen_clicked();
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();

private:
    Ui::Preferences* m_ui;
    void loadSettings() noexcept;
};
