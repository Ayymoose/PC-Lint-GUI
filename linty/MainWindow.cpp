/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QTimer>
#include "Jenkins.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Worker.h"
#include "Linter.h"
#include "ProgressWindow.h"
#include <QThread>
#include "ProjectSolution.h"
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{

    qRegisterMetaType<LINTER_STATUS>("LINTER_STATUS");
    qRegisterMetaType<QSet<lintMessage>>("QSet<lintMessage>");

    // Turn UI into actual objects
    m_ui->setupUi(this);

    connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(m_ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(m_ui->actionCut, &QAction::triggered, this, &MainWindow::cut);

    m_lintOptions = new LintOptions(this);
    // Load any settings we have
    m_lintOptions->loadSettings();

    // Load icons
    m_icons.loadIcons();

    // Configure the lint table
    configureLintTable();

    // Configure the code editor
    m_ui->codeEditor->setLineNumberAreaColour(LINE_NUMBER_AREA_COLOUR);
    m_ui->codeEditor->setLineNumberBackgroundColour(LINE_CURRENT_BACKGROUND_COLOUR);

    // With syntax highlighting
    m_highlighter = new Highlighter(m_ui->codeEditor->document());

    m_linter = new Linter;

    // Status bar labels
    m_ui->statusBar->addPermanentWidget(m_ui->label);
    m_ui->codeEditor->setLabel(m_ui->label);

    // | Errors: 0 Warnings: 0 Info: 0 |

    // Toggled on (show messages only of this type)
    // Toggle off (hide messages only of this type)
    m_toggleError = true;
    m_toggleInfo = true;
    m_toggleWarning = true;

    m_lowerToolbar = new QToolBar;
    m_buttonErrors = new QToolButton;
    m_buttonWarnings = new QToolButton;
    m_buttonInfo = new QToolButton;

    m_buttonErrors->setCheckable(true);
    m_buttonErrors->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionError = new QAction;
    m_actionError->setIcon(QIcon(":/images/error.png"));
    m_actionError->setText("Errors: 0");
    m_buttonErrors->setDefaultAction(m_actionError);
    m_actionError->setCheckable(true);
    m_actionError->setChecked(m_toggleError);

    m_buttonWarnings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionWarning = new QAction;
    m_actionWarning->setIcon(QIcon(":/images/warning.png"));
    m_actionWarning->setText("Warnings: 0");
    m_buttonWarnings->setDefaultAction(m_actionWarning);
    m_actionWarning->setCheckable(true);
    m_actionWarning->setChecked(m_toggleWarning);

    m_buttonInfo->setCheckable(true);
    m_buttonInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionInfo = new QAction;
    m_actionInfo->setIcon(QIcon(":/images/info.png"));
    m_actionInfo->setText("Information: 0");
    m_buttonInfo->setDefaultAction(m_actionInfo);
    m_actionInfo->setCheckable(true);
    m_actionInfo->setChecked(m_buttonInfo);

    m_ui->verticalLayout_2->addWidget(m_lowerToolbar);
    m_lowerToolbar->addWidget(m_buttonErrors);
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonWarnings);
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonInfo);
    m_lowerToolbar->addSeparator();

    connect(m_actionError, &QAction::triggered, this, [this](bool checked)
    {
        m_toggleError = checked;
        populateLintTable();
    });

    connect(m_actionInfo, &QAction::triggered, this, [this](bool checked)
    {
        m_toggleInfo = checked;
        populateLintTable();
    });

    connect(m_actionWarning, &QAction::triggered, this, [this](bool checked)
    {
        m_toggleWarning = checked;
        populateLintTable();
    });

    connect(this, &MainWindow::signalUpdateTypes, this, [this](int errors, int warnings, int info)
    {
        m_actionError->setText("Errors: " + QString::number(errors));
        m_actionWarning->setText("Warnings: " + QString::number(warnings));
        m_actionInfo->setText("Information: " + QString::number(info));
    });
}


void MainWindow::configureLintTable()
{

    // Icon column width
    m_ui->lintTable->setColumnWidth(0,24);

    // Code width
    m_ui->lintTable->setColumnWidth(1,40);

    // Message width
    m_ui->lintTable->setColumnWidth(2,800);

}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_highlighter;
    delete m_linter;
    delete m_actionError;
    delete m_buttonErrors;
    delete m_actionWarning;
    delete m_buttonWarnings;
    delete m_actionInfo;
    delete m_buttonInfo;
    delete m_lowerToolbar;
    delete m_lintOptions;
}

void MainWindow::slotUpdateLintTable()
{
    populateLintTable();
}

void MainWindow::save()
{
    QString currentFile = m_ui->codeEditor->loadedFile();
    // If we have a loaded file then save it
    if (!currentFile.isEmpty())
    {
        QFile file(currentFile);
        if (!file.open(QIODevice::WriteOnly | QFile::Text))
        {
            QMessageBox::critical(this, "Error", "Cannot save file: " + file.errorString());
            return;
        }
        QTextStream out(&file);
        QString text = m_ui->codeEditor->toPlainText();
        out << text;
        file.close();

        m_ui->statusBar->showMessage("Saved " + currentFile);
    }
}

void MainWindow::exit()
{
    QCoreApplication::quit();
}

void MainWindow::copy()
{

}

void MainWindow::cut()
{

}

void MainWindow::on_actionLint_options_triggered()
{
    m_lintOptions->setModal(true);
    m_lintOptions->loadSettings();
    m_lintOptions->exec();

}

void MainWindow::populateLintTable()
{
    // Populate the table view with all the lint messages
    // Clear all existing entries

    QTableWidget* lintTable = m_ui->lintTable;
    lintTable->setSortingEnabled(false);
    int rowCount = lintTable->rowCount();
    int colCount = lintTable->columnCount();

    auto lintMessages = m_linter->getLinterMessages();

    int progress = 0;
    int maxProgress = (rowCount * colCount) + lintMessages.size();
    emit signalUpdateProgress(progress);
    emit signalUpdateProgressMax(maxProgress);
    emit signalUpdateStatus("Updating table...");

    for (int row=0; row < rowCount; row++)
    {
        for (int col=0; col < colCount; col++)
        {
            QTableWidgetItem* item = lintTable->item(row,col);
            delete item;
            emit signalUpdateProgress(progress++);
        }
    }

    lintTable->clearContents();
    lintTable->setRowCount(0);

    for (const lintMessage& message : lintMessages)
    {
        QString code = message.code;
        QString file = message.file;
        QString line = message.line;
        QString type = message.type;
        QString description = message.description;


        MESSAGE_TYPE messageType;

        // Determine type
        if (!QString::compare(type, TYPE_ERROR, Qt::CaseInsensitive))
        {
            messageType = MESSAGE_TYPE_ERROR;
        }
        else if (!QString::compare(type, TYPE_WARNING, Qt::CaseInsensitive))
        {
            messageType = MESSAGE_TYPE_WARNING;
        }
        else if (!QString::compare(type, TYPE_INFORMATION, Qt::CaseInsensitive))
        {
            messageType = MESSAGE_TYPE_INFORMATION;
        }
        else
        {
            messageType = MESSAGE_TYPE_UNKNOWN;
        }


        // Filter
        if (!m_toggleError)
        {
            if (messageType == MESSAGE_TYPE_ERROR)
            {
                continue;
            }
        }

        if (!m_toggleWarning)
        {
            if (messageType == MESSAGE_TYPE_WARNING)
            {
                continue;
            }
        }

        if (!m_toggleInfo)
        {
            if (messageType == MESSAGE_TYPE_INFORMATION)
            {
                continue;
            }
        }

        // Insert row
        lintTable->insertRow(lintTable->rowCount());

        // Set item data
        QTableWidgetItem* typeWidget = new QTableWidgetItem;
        QTableWidgetItem* codeWidget = new QTableWidgetItem;
        QTableWidgetItem* lineWidget = new QTableWidgetItem;
        QTableWidgetItem* fileWidget = new QTableWidgetItem;

        codeWidget->setData(Qt::DisplayRole,code.toUInt());
        lineWidget->setData(Qt::DisplayRole,line.toUInt());
        fileWidget->setData(Qt::DisplayRole, QFileInfo(file).fileName());
        fileWidget->setData(Qt::UserRole, file);


        QImage* icon = nullptr;
        switch (messageType)
        {
            case MESSAGE_TYPE_ERROR: icon = m_icons[ICON_ERROR];  break;
            case MESSAGE_TYPE_WARNING: icon = m_icons[ICON_WARNING]; break;
            case MESSAGE_TYPE_INFORMATION: icon = m_icons[ICON_INFORMATION]; break;
            case MESSAGE_TYPE_UNKNOWN: icon = m_icons[ICON_UNKNOWN]; break;
        }

        if (icon)
        {
            typeWidget->setData(Qt::DecorationRole, QPixmap::fromImage(*icon));
        }

        lintTable->setItem( lintTable->rowCount()-1, 0, typeWidget);
        lintTable->setItem( lintTable->rowCount()-1, 1, codeWidget);
        lintTable->setItem( lintTable->rowCount()-1, 2, new QTableWidgetItem(description));
        lintTable->setItem( lintTable->rowCount()-1, 3, fileWidget);
        lintTable->setItem( lintTable->rowCount()-1, 4, lineWidget);
        emit signalUpdateProgress(progress++);
    }
    lintTable->setSortingEnabled(true);

    // Show message if there are no lint problems
    if (m_linter->numberOfErrors() == 0 && m_linter->numberOfWarnings() == 0 && m_linter->numberOfInfo() == 0)
    {
        // Set item data
        auto type = new QTableWidgetItem;
        type->setData(Qt::DecorationRole, QPixmap::fromImage(*m_icons[ICON_CORRECT]));
        lintTable->setItem( lintTable->rowCount()-1, 0, type);
        lintTable->setItem( lintTable->rowCount()-1, 2, new QTableWidgetItem("No errors were detected :)"));
    }

    emit signalUpdateTypes(m_linter->numberOfErrors(), m_linter->numberOfWarnings(), m_linter->numberOfInfo());
    emit signalLintComplete();
}

bool MainWindow::verifyLint()
{
    QDir path;
    QFileInfo fileInfo;

    // Check if executable exists
    QString linterExecutable = m_lintOptions->getLinterExecutablePath().trimmed();
    fileInfo.setFile(linterExecutable);

    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "Lint executable does not exist: '" + linterExecutable + "'");
        return false;
    }

    if (!fileInfo.isExecutable())
    {
        QMessageBox::critical(this,"Error", "Non-executable file specified: '" + linterExecutable + "'");
        return false;
    }

    // Check if lint file exists
    QString linterLintFile = m_lintOptions->getLinterLintFilePath().trimmed();
    fileInfo.setFile(linterLintFile);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "Lint file does not exist: '" + linterLintFile + "'");
        return false;
    }

    // Check if the directory exists
    QString lintDirectory = m_lintOptions->getLinterDirectory().trimmed();
    path.setPath(lintDirectory);
    if (!path.exists())
    {
        QMessageBox::critical(this,"Error", "Directory does not exist: '" + lintDirectory + "'");
        return false;
    }

    return true;
}

void MainWindow::startLint(bool lintProject)
{
    // This function tries to call the linter
    // The linter will attempt to lint the files specified in the directory we give it
    // It will show all output in a list view where we can view it
    // Should be in a separate thread really


    if (verifyLint())
    {
        QList<QString> directoryFiles;
        QString fileName;

        // Lint a project solution file
        if (lintProject)
        {
            fileName = QFileDialog::getOpenFileName(this, "Select project file", LintOptions::m_lastDirectory, "Atmel 7 studio (*.cproj)");
            if (fileName != "")
            {
                LintOptions::m_lastDirectory = QFileInfo(fileName).absolutePath();
                // Currently only Atmel Studio 7 project supported
                AtmelStudio7ProjectSolution as7ProjectSolution;
                // TODO: VS project linting solution
                try
                {
                    directoryFiles = as7ProjectSolution.buildSourceFiles(fileName);
                    m_lastProjectLoaded = fileName;
                    m_directoryFiles = directoryFiles;
                } catch (const std::logic_error& e)
                {
                    QMessageBox::critical(this,"Error", QString(e.what()));
                }

            }
        }
        else
        {
            // Lint a directory containing some source file(s)
            QDirIterator dirIterator(m_lintOptions->getLinterDirectory().trimmed(), QStringList() << "*.c");
            while (dirIterator.hasNext())
            {
                directoryFiles.append(dirIterator.next());
            }
        }
        //

        // Only start linting if a file was selected
        if (directoryFiles.size())
        {
            m_linter->setLinterFile(m_lintOptions->getLinterLintFilePath().trimmed());
            m_linter->setLinterExecutable(m_lintOptions->getLinterExecutablePath().trimmed());
            m_linter->setLintFiles(directoryFiles);

            // Display directory name or filename
            if (fileName != "")
            {
                // Lint a project solution
                startLintThread(QFileInfo(fileName).fileName());
            }
            else
            {
                // Start linting a directory
                startLintThread(m_lintOptions->getLinterDirectory().trimmed());
            }
        }
    }

}

void MainWindow::on_lintTable_cellDoubleClicked(int row, int)
{
    // Get the file
    // Column 3 is just the file name
    QTableWidgetItem* item = m_ui->lintTable->item(row,3);

    QString fileToLoad = item->data(Qt::UserRole).value<QString>();

    if (!fileToLoad.isEmpty())
    {
        if ((fileToLoad != m_ui->codeEditor->loadedFile()))
        {
            DEBUG_LOG("Loading file: " + fileToLoad);

            // Load the file into the code editor
            m_ui->codeEditor->loadFile(fileToLoad);

            // Update the status bar
            m_ui->statusBar->showMessage("Loaded " + fileToLoad);
        }

        // Select the line number
        item = m_ui->lintTable->item(row,4);
        uint32_t lineNumber = item->text().toUInt();
        m_ui->codeEditor->selectLine(lineNumber);
    }

}

void MainWindow::on_actionLint_project_triggered()
{
    startLint(true);
}

void MainWindow::on_actionLint_triggered()
{
    startLint(false);
}

void MainWindow::slotLintFinished(LINTER_STATUS status)
{
    switch (status)
    {
    case LINTER_UNSUPPORTED_VERSION:
        QMessageBox::critical(this,"Error", "Unsupported lint version: '" + m_linter->getLinterExecutable() + "'");
        break;
    case LINTER_FAIL:
        QMessageBox::critical(this,"Error", "Failed to run lint executable: '" + m_linter->getLinterExecutable() + "'");
        break;
    case LINTER_ERROR:
        QMessageBox::critical(this,"Error", "Linter encountered an error!");
        break;
    case LINTER_OK:
        populateLintTable();
        break;
    }
}

void MainWindow::startLintThread(QString title)
{
    m_progressWindow = new ProgressWindow(this);
    m_progressWindow->setModal(true);
    m_progressWindow->lintProcess();
    emit signalUpdateProgressTitle(title);
    m_progressWindow->exec();
    delete m_progressWindow;
}

void MainWindow::on_aboutLinty_triggered()
{
    QMessageBox versionMessageBox(this);
    versionMessageBox.setWindowTitle("Information");
    versionMessageBox.setText
            (
                "Version: " BUILD_VERSION "\n"
                "Date: " BUILD_DATE "\n"
                "Commit: " BUILD_COMMIT "\n"
                );
    versionMessageBox.exec();
}

void MainWindow::on_actionRefresh_triggered()
{
    // If we have a project name that was already loaded
    // Then try to lint that again
    if (m_lastProjectLoaded != "")
    {
        m_linter->setLinterFile(m_lintOptions->getLinterLintFilePath().trimmed());
        m_linter->setLinterExecutable(m_lintOptions->getLinterExecutablePath().trimmed());
        m_linter->setLintFiles(m_directoryFiles);
        startLintThread(QFileInfo(m_lastProjectLoaded).fileName());
    }
}
