#include "LintOptions.h"
#include "ui_LintOptions.h"
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include "Settings.h"
#include "Log.h"

LintOptions::LintOptions(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::LintOptions)
{
    m_ui->setupUi(this);
    // Hide "?" on Window
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


}



QString LintOptions::getLinterExecutablePath() const
{
    return m_ui->lintInputText->text();
}

QString LintOptions::getLinterLintFilePath() const
{
    return m_ui->lintFileInputText->text();
}

QString LintOptions::getLinterLintOptions() const
{
   return m_ui->lintOptionsInputText->text();
}

QString LintOptions::getLinterDirectory() const
{
    return m_ui->sourceFileInputText->text();
}


void LintOptions::loadSettings()
{
    // Load settings
    QMap<QString,QString> keyValues = Settings::loadINISettings();

    m_ui->lintInputText->setText(keyValues["LINT_EXE"]); // The path to the linter exe
    m_ui->lintFileInputText->setText(keyValues["LINT_FILE"]); // The path to the lint file (including the file itself)
    m_ui->lintOptionsInputText->setText(keyValues["LINT_COMMANDS"]); // Lint commands
    m_ui->sourceFileInputText->setText(keyValues["LINT_SOURCE"]); // Lint source

    //qDebug() << keyValues;
}


LintOptions::~LintOptions()
{
    delete m_ui;
}

void LintOptions::on_sourceButton_clicked()
{
    QFileDialog dialogue(this);
    dialogue.setFileMode(QFileDialog::Directory);
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "D:\\", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->sourceFileInputText->setText(directory);
}

void LintOptions::on_buttonBox_rejected()
{
    this->close();
}

void LintOptions::on_buttonBox_accepted()
{
    // Save our options to file

    Settings::writeINI("LINT_EXE", m_ui->lintInputText->text()); // The path to the linter
    Settings::writeINI("LINT_FILE", m_ui->lintFileInputText->text()); // The path to the lint file (including the file itself)
    Settings::writeINI("LINT_COMMANDS", m_ui->lintOptionsInputText->text()); // Lint commands
    Settings::writeINI("LINT_SOURCE", m_ui->sourceFileInputText->text()); // Lint source

    Log::log("test write");

    this->close();
}

void LintOptions::on_lintButton_clicked()
{
    QFileDialog dialogue(this);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select lint executable"), "D:\\");
    m_ui->lintInputText->setText(fileName);
}

void LintOptions::on_lintFileButton_clicked()
{
    QFileDialog dialogue(this);
    dialogue.setFileMode(QFileDialog::Directory);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select lint (.lnt) file "), "D:\\");
    m_ui->lintFileInputText->setText(fileName);
}
