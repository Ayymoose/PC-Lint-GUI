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

#include "Preferences.h"
#include "ui_Preferences.h"
#include "Log.h"
#include "MainWindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QThread>
#include <QProcess>

namespace
{
const char* PC_LINT = "PC-lint";
const char* PC_LINT_PLUS = "PC-lint Plus";
}


QString Preferences::m_lastDirectory = "";

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Preferences)
{
    m_ui->setupUi(this);
    // Hide "?" on Window
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QObject::connect(this, &Preferences::signalLintVersion, static_cast<MainWindow*>(parent), &MainWindow::slotLintVersion);

    // Allow user to select number of threads to use for lint process
    int hardwareThreads = QThread::idealThreadCount();
    m_ui->labelMaxHardwareThreads->setText(" / " + QString::number(hardwareThreads));
    m_ui->lintUsingThreadsComboBox->clear();
    for (int i=1; i<=hardwareThreads; i++)
    {
        m_ui->lintUsingThreadsComboBox->addItem(QString::number(i));
    }


    // TODO: Determine PC-Lint or PC-Lint Plus version and send to main linter


    // TODO: Add default editor to launch log

    m_ui->preferencesTree->setColumnCount(1);

    // Load settings
    loadSettings();

    // If the last directory opened is nothing then set it to the first available drive
    if (m_lastDirectory.isEmpty())
    {
        m_lastDirectory = QDir::drives().first().path();
    }
}

Preferences::~Preferences()
{
    delete m_ui;
}

QString Preferences::getLinterExecutablePath() const noexcept
{
    return m_ui->lintPathExeLineEdit->text();
}

QString Preferences::getLinterLintFilePath() const noexcept
{
    return m_ui->lintFileLineEdit->text();
}

int Preferences::getLinterHardwareThreads() const noexcept
{
    return m_ui->lintUsingThreadsComboBox->currentIndex();
}

void Preferences::on_lintPathFileOpen_clicked()
{
    QFileDialog dialogue(this);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select PC-Lint/PC-Lint Plus executable"), m_lastDirectory, "PC-Lint/PC-Lint Plus executable (*.exe)");
    m_lastDirectory = QFileInfo(fileName).absolutePath();
    if (!fileName.trimmed().isEmpty())
    {
        m_ui->lintPathExeLineEdit->setText(fileName);
    }
}

void Preferences::on_lintFileFileOpen_clicked()
{
    QFileDialog dialogue(this);
    dialogue.setFileMode(QFileDialog::Directory);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select PC-Lint/PC-Lint Plus file "), m_lastDirectory, "PC-Lint/PC-Lint Plus file (*.lnt)");
    m_lastDirectory = QFileInfo(fileName).absolutePath();
    if (!fileName.trimmed().isEmpty())
    {
        m_ui->lintFileLineEdit->setText(fileName);
    }
}

// TODO: This must be inside the lint
void Preferences::checkPCLintVersion() noexcept
{
    QProcess lintProcess;
    lintProcess.setProgram(m_ui->lintPathExeLineEdit->text());
    lintProcess.start();

    PCLint::Version version = PCLint::VERSION_UNKNOWN;

    QObject::connect(&lintProcess, &QProcess::errorOccurred, this, [&](const QProcess::ProcessError& error)
    {
        qDebug() << __FUNCTION__ << " error occurred: " << error;
    });

    QObject::connect(&lintProcess, &QProcess::readyReadStandardError, this,[&]()
    {
        auto const readStdErr = lintProcess.readAllStandardError();
        if (readStdErr.contains(PC_LINT_PLUS))
        {
            version = PCLint::VERSION_PC_LINT_PLUS;
        }
        else if (readStdErr.contains(PC_LINT))
        {
            version = PCLint::VERSION_PC_LINT;
        }
        lintProcess.closeReadChannel(QProcess::StandardError);
        lintProcess.close();
    });

    lintProcess.waitForStarted();
    lintProcess.waitForFinished();

    if (version != PCLint::VERSION_UNKNOWN)
    {
        emit signalLintVersion(version);
    }
    else
    {
        qWarning() << "Couldn't determine lint version";
    }
}

// Save clicked
void Preferences::on_buttonSave_clicked()
{
    // Save all settings
    QSettings settings(PCLint::SETTINGS_APPLICATION_NAME,QSettings::IniFormat);
    settings.beginGroup(PCLint::SETTINGS_GROUP_NAME);
    settings.setValue(PCLint::SETTINGS_MAX_THREADS, m_ui->lintUsingThreadsComboBox->currentText());
    settings.setValue(PCLint::SETTINGS_LINT_EXECUTABLE_PATH, m_ui->lintPathExeLineEdit->text());
    settings.setValue(PCLint::SETTINGS_LINT_FILE_PATH, m_ui->lintFileLineEdit->text());
    settings.setValue(PCLint::SETTINGS_LAST_DIRECTORY, m_lastDirectory);
    settings.endGroup();

    // Determine if we are using PC-Lint or PC-Lint Plus
    checkPCLintVersion();

    close();
}

void Preferences::loadSettings() noexcept
{
    QSettings settings(PCLint::SETTINGS_APPLICATION_NAME,QSettings::IniFormat);
    settings.beginGroup(PCLint::SETTINGS_GROUP_NAME);
    // Set default threads to 1 if not selected
    auto const lintThreads = std::max(settings.value(PCLint::SETTINGS_MAX_THREADS).toInt()-1,0);
    m_ui->lintUsingThreadsComboBox->setCurrentIndex(lintThreads);
    m_ui->lintPathExeLineEdit->setText(settings.value(PCLint::SETTINGS_LINT_EXECUTABLE_PATH).toString());
    m_ui->lintFileLineEdit->setText(settings.value(PCLint::SETTINGS_LINT_FILE_PATH).toString());
    m_lastDirectory = settings.value(PCLint::SETTINGS_LAST_DIRECTORY).toString();
    settings.endGroup();
}

// Cancel clicked
void Preferences::on_buttonCancel_clicked()
{
    close();
    loadSettings();
}

// TODO: Reload events on X, doesn't close dialog if reject() overriden for some reason
// X clicked
/*void Preferences::reject()
{
    close();
    loadSettings();
}
*/
