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
    m_ui->setupUi(this);
    this->setCentralWidget(m_ui->textEdit);

    connect(m_ui->actionNew, &QAction::triggered, this, &MainWindow::newDocument);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(m_ui->actionSave_as, &QAction::triggered, this, &MainWindow::saveAs);
    connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(m_ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(m_ui->actionCut, &QAction::triggered, this, &MainWindow::cut);
    connect(m_ui->actionPaste, &QAction::triggered, this, &MainWindow::paste);
    connect(m_ui->actionUndo, &QAction::triggered, this, &MainWindow::undo);
    connect(m_ui->actionRedo, &QAction::triggered, this, &MainWindow::redo);
    connect(m_ui->actionFont, &QAction::triggered, this, &MainWindow::selectFont);
    connect(m_ui->actionBold, &QAction::triggered, this, &MainWindow::setFontBold);
    connect(m_ui->actionUnderline, &QAction::triggered, this, &MainWindow::setFontUnderline);
    connect(m_ui->actionItalic, &QAction::triggered, this, &MainWindow::setFontItalic);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);

    // Load any settings we have
    m_lintOptions.loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::newDocument()
{
  //  currentFile.clear();
   // m_ui->textEdit->setText(QString());
}

void MainWindow::open()
{
  /*  QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    m_ui->textEdit->setText(text);
    file.close();*/
}

void MainWindow::save()
{
   /* QString fileName;
    // If we don't have a filename from before, get one.
    if (currentFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save");
        currentFile = fileName;
    } else {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = m_ui->textEdit->toPlainText();
    out << text;
    file.close();*/
}

void MainWindow::saveAs()
{
  /*  QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = m_ui->textEdit->toPlainText();
    out << text;
    file.close();*/
}

void MainWindow::print()
{

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

void MainWindow::undo()
{
   //  m_ui->textEdit->undo();
}

void MainWindow::redo()
{
   // m_ui->textEdit->redo();
}

void MainWindow::selectFont()
{
    /*bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        m_ui->textEdit->setFont(font);*/
}

void MainWindow::setFontUnderline(bool underline)
{
    m_ui->textEdit->setFontUnderline(underline);
}

void MainWindow::setFontItalic(bool italic)
{
  //  m_ui->textEdit->setFontItalic(italic);
}

void MainWindow::setFontBold(bool bold)
{
    /*bold ? m_ui->textEdit->setFontWeight(QFont::Bold) :
           m_ui->textEdit->setFontWeight(QFont::Normal);*/
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

    Linter linter;
    LINTER_STATUS linterStatus = linter.lint(linterExecutable,linterLintFile, linterLintCommands, linterLintDirectory);
    switch (linterStatus)
    {
    case LINTER_EXECUTABLE_UNKNOWN:
        QMessageBox::critical(this,"Error", "Unknown lint executable specified: '" + linterExecutable + "'");
        break;
    case LINTER_TIMEOUT:
        QMessageBox::critical(this,"Error", "Timed out running lint executable: '" + linterExecutable + "'");
        break;
    case LINTER_ERROR:
        QMessageBox::critical(this,"Error", "Linter encountered an error!");
        break;
    case LINTER_OK:

        break;
    }
}
