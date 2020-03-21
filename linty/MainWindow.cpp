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

#include "MainWindow.h"
#include "ui_MainWindow.h"

Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Notepad)
{
    m_ui->setupUi(this);
    this->setCentralWidget(m_ui->textEdit);

    connect(m_ui->actionNew, &QAction::triggered, this, &Notepad::newDocument);
    connect(m_ui->actionOpen, &QAction::triggered, this, &Notepad::open);
    connect(m_ui->actionSave, &QAction::triggered, this, &Notepad::save);
    connect(m_ui->actionSave_as, &QAction::triggered, this, &Notepad::saveAs);
    connect(m_ui->actionExit, &QAction::triggered, this, &Notepad::exit);
    connect(m_ui->actionCopy, &QAction::triggered, this, &Notepad::copy);
    connect(m_ui->actionCut, &QAction::triggered, this, &Notepad::cut);
    connect(m_ui->actionPaste, &QAction::triggered, this, &Notepad::paste);
    connect(m_ui->actionUndo, &QAction::triggered, this, &Notepad::undo);
    connect(m_ui->actionRedo, &QAction::triggered, this, &Notepad::redo);
    connect(m_ui->actionFont, &QAction::triggered, this, &Notepad::selectFont);
    connect(m_ui->actionBold, &QAction::triggered, this, &Notepad::setFontBold);
    connect(m_ui->actionUnderline, &QAction::triggered, this, &Notepad::setFontUnderline);
    connect(m_ui->actionItalic, &QAction::triggered, this, &Notepad::setFontItalic);
    connect(m_ui->actionAbout, &QAction::triggered, this, &Notepad::about);

}

Notepad::~Notepad()
{
    delete m_ui;
}

void Notepad::newDocument()
{
    currentFile.clear();
    m_ui->textEdit->setText(QString());
}

void Notepad::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
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
    file.close();
}

void Notepad::save()
{
    QString fileName;
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
    file.close();
}

void Notepad::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
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
    file.close();
}

void Notepad::print()
{

}

void Notepad::exit()
{
    QCoreApplication::quit();
}

void Notepad::copy()
{

}

void Notepad::cut()
{

}

void Notepad::paste()
{

}

void Notepad::undo()
{
     m_ui->textEdit->undo();
}

void Notepad::redo()
{
    m_ui->textEdit->redo();
}

void Notepad::selectFont()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        m_ui->textEdit->setFont(font);
}

void Notepad::setFontUnderline(bool underline)
{
    m_ui->textEdit->setFontUnderline(underline);
}

void Notepad::setFontItalic(bool italic)
{
    m_ui->textEdit->setFontItalic(italic);
}

void Notepad::setFontBold(bool bold)
{
    bold ? m_ui->textEdit->setFontWeight(QFont::Bold) :
           m_ui->textEdit->setFontWeight(QFont::Normal);
}

void Notepad::about()
{
   QMessageBox::about(this, tr("About MDI"),
                tr("The <b>Notepad</b> example demonstrates how to code a basic "
                   "text editor using QtWidgets"));

}

void Notepad::on_actionLint_options_triggered()
{
    m_lintOptions.setModal(true);
    m_lintOptions.loadSettings();
    m_lintOptions.exec();

}
