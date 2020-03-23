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
#include <QFont>
#include <QFontDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Linter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{

    // Turn UI into actual objects
    m_ui->setupUi(this);

    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(m_ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(m_ui->actionCut, &QAction::triggered, this, &MainWindow::cut);
    connect(m_ui->actionPaste, &QAction::triggered, this, &MainWindow::paste);

    // Load any settings we have
    m_lintOptions.loadSettings();

    // Load icons
    m_icons.loadIcons();

    // Configure the lint table
    configureLintTable();

    // Configure the code editor
    m_ui->codeEditor->setLineNumberAreaColour(LINE_NUMBER_AREA_COLOUR);
    m_ui->codeEditor->setLineNumberBackgroundColour(LINE_CURRENT_BACKGROUND_COLOUR);


    m_highlighter = new Highlighter(m_ui->codeEditor->document());
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
}

void MainWindow::open()
{

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

void MainWindow::paste()
{

}

void MainWindow::about()
{
   /*QMessageBox::about(this, tr("About MDI"),
                tr("The <b>Notepad</b> example demonstrates how to code a basic "
                   "text editor using QtWidgets"));*/

}

void MainWindow::on_actionLint_options_triggered()
{
    m_lintOptions.setModal(true);
    m_lintOptions.loadSettings();
    m_lintOptions.exec();

}

void MainWindow::populateLintTable(const QList<lintMessage>& lintMessages)
{
    // Populate the table view with all the lint messages

    // Clear all existing entries


    QTableWidget* lintTable = m_ui->lintTable;

    for (int row=0; row<lintTable->rowCount(); row++)
    {
        for (int col=0; col<lintTable->columnCount(); col++)
        {
            QTableWidgetItem* item = lintTable->item(row,col);
            delete item;
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

        // Insert row
        lintTable->insertRow(lintTable->rowCount());

        // Set item data
        QTableWidgetItem* typeWidget = new QTableWidgetItem;

        QImage* icon = nullptr;

        if (type == TYPE_ERROR)
        {
            icon = m_icons[ICON_ERROR];
        }
        else if (type == TYPE_WARNING)
        {
            icon = m_icons[ICON_WARNING];
        }
        else if (type == TYPE_INFORMATION)
        {
            icon = m_icons[ICON_INFORMATION];
        }
        else
        {
            qDebug() << "Unknown type encountered";
        }

        Q_ASSERT(icon != nullptr);

        typeWidget->setData(Qt::DecorationRole, QPixmap::fromImage(*icon));
        lintTable->setItem( lintTable->rowCount()-1, 0, typeWidget);
        lintTable->setItem( lintTable->rowCount()-1, 1, new QTableWidgetItem(code));
        lintTable->setItem( lintTable->rowCount()-1, 2, new QTableWidgetItem(description));
        lintTable->setItem( lintTable->rowCount()-1, 3, new QTableWidgetItem(QFileInfo(file).fileName()));
        lintTable->setItem( lintTable->rowCount()-1, 4, new QTableWidgetItem(line));
    }

    // Don't forget to free the memoryyyy

}


void MainWindow::on_actionLint_triggered()
{
    // This function tries to call the linter
    // The linter will attempt to lint the files specified in the directory we give it
    // It will show all output in a list view where we can view it
    // Should be in a separate thread really

    QDir path;
    QFileInfo fileInfo;

    // Check if executable exists
    QString linterExecutable = m_lintOptions.getLinterExecutablePath().trimmed();
    fileInfo.setFile(linterExecutable);

    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "Lint executable does not exist: '" + linterExecutable + "'");
        return;
    }

    if (!fileInfo.isExecutable())
    {
        QMessageBox::critical(this,"Error", "Non-executable file specified: '" + linterExecutable + "'");
        return;
    }

    // Check if lint file exists
    QString linterLintFile = m_lintOptions.getLinterLintFilePath().trimmed();
    fileInfo.setFile(linterLintFile);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "Lint file does not exist: '" + linterLintFile + "'");
        return;
    }

    // Check if the directory exists
    QString linterLintDirectory = m_lintOptions.getLinterDirectory().trimmed();
    path.setPath(linterLintDirectory);
    if (!path.exists())
    {
        QMessageBox::critical(this,"Error", "Directory does not exist: '" + linterLintDirectory + "'");
        return;
    }

    // Mmmm?
    QString linterLintCommands = m_lintOptions.getLinterLintOptions().trimmed();
    QList<lintMessage> lintMessages;

    LINTER_STATUS linterStatus = m_linter.lint(linterExecutable,linterLintFile, linterLintCommands, linterLintDirectory, lintMessages);
    switch (linterStatus)
    {
    case LINTER_UNSUPPORTED_VERSION:
        QMessageBox::critical(this,"Error", "Unsupported lint version: '" + linterExecutable + "'");
        break;
    case LINTER_FAIL:
        QMessageBox::critical(this,"Error", "Failed to run lint executable: '" + linterExecutable + "'");
        break;
    case LINTER_ERROR:
        QMessageBox::critical(this,"Error", "Linter encountered an error!");
        break;
    case LINTER_OK:
        populateLintTable(lintMessages);
        break;
    }
}

void MainWindow::on_lintTable_cellDoubleClicked(int row, int column)
{
    // If we click on the message column
    if (column == 2)
    {
        // Get the file
        QTableWidgetItem* item = m_ui->lintTable->item(row,column+1);

        QString fileToLoad = m_linter.getLintingDirectory() + "/" + item->text();

        qDebug() << "Loading file: " << fileToLoad;

        // Load the file into the code editor
        m_ui->codeEditor->loadFile(fileToLoad);

        // Select the line
        item = m_ui->lintTable->item(row,column+2);
        uint32_t lineNumber = item->text().toUInt();
        m_ui->codeEditor->selectLine(lineNumber);

        // Update the status bar
        m_ui->statusBar->showMessage("Loaded " + fileToLoad);

    }
}
