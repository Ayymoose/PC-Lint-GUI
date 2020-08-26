#include "Preferences.h"
#include "ui_Preferences.h"
#include "Log.h"

#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QThread>

QString Preferences::m_lastDirectory = "";

namespace
{
    const QString SETTINGS_APPLICATION_NAME = "Linty";
    const QString SETTINGS_GROUP_NAME = "Settings";
    const QString SETTINGS_MAX_THREADS = "MaxThreads";
    const QString SETTINGS_LINT_EXECUTABLE_PATH = "LintExecutablePath";
    const QString SETTINGS_LINT_FILE_PATH = "LintFilePath";
    const QString SETTINGS_LAST_DIRECTORY = "LastDirectory";
}

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Preferences)
{
    m_ui->setupUi(this);
    // Hide "?" on Window
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


    // Allow user to select number of threads to use for lint process
    int hardwareThreads = QThread::idealThreadCount();
    m_ui->labelMaxHardwareThreads->setText(" / " + QString::number(hardwareThreads));
    m_ui->lintUsingThreadsComboBox->clear();
    for (int i=1; i<=hardwareThreads; i++)
    {
        m_ui->lintUsingThreadsComboBox->addItem(QString::number(i));
    }

    m_ui->preferencesTree->setColumnCount(1);

    // Load settings
    QSettings settings(SETTINGS_APPLICATION_NAME,QSettings::IniFormat);
    settings.beginGroup(SETTINGS_GROUP_NAME);
    m_ui->lintUsingThreadsComboBox->setCurrentIndex(settings.value(SETTINGS_MAX_THREADS).toInt()-1);
    m_ui->lintPathExeLineEdit->setText(settings.value(SETTINGS_LINT_EXECUTABLE_PATH).toString());
    m_ui->lintFileLineEdit->setText(settings.value(SETTINGS_LINT_FILE_PATH).toString());
    m_lastDirectory = settings.value(SETTINGS_LAST_DIRECTORY).toString();
    settings.endGroup();

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

// Save clicked
void Preferences::on_buttonSave_clicked()
{
    // Save all settings
    QSettings settings(SETTINGS_APPLICATION_NAME,QSettings::IniFormat);
    settings.beginGroup(SETTINGS_GROUP_NAME);
    settings.setValue(SETTINGS_MAX_THREADS, m_ui->lintUsingThreadsComboBox->currentText());
    settings.setValue(SETTINGS_LINT_EXECUTABLE_PATH, m_ui->lintPathExeLineEdit->text());
    settings.setValue(SETTINGS_LINT_FILE_PATH, m_ui->lintFileLineEdit->text());
    settings.setValue(SETTINGS_LAST_DIRECTORY, m_lastDirectory);
    settings.endGroup();
    close();
}

// Cancel clicked
void Preferences::on_buttonCancel_clicked()
{
    close();
}
