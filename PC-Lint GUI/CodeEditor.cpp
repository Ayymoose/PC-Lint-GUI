/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
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

#include "CodeEditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QScrollBar>
#include "Log.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    // Line number area
    m_lineNumberArea = std::make_unique<LineNumberArea>(this);
    m_lineNumberAreaColour = LINE_NUMBER_AREA_COLOUR;
    m_lineNumberBackgroundColour = LINE_CURRENT_BACKGROUND_COLOUR;
    m_zoomLabel = nullptr;

    m_highlightError = false;
    this->setFont(QFont("Consolas",14));

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    setReadOnly(true);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::setLineNumberAreaColour(const QColor& colour) noexcept
{
    m_lineNumberAreaColour = colour;
}

void CodeEditor::setLineNumberBackgroundColour(const QColor& colour) noexcept
{
    m_lineNumberBackgroundColour = colour;
}

CodeEditor::~CodeEditor()
{
}

QString CodeEditor::loadedFile() const noexcept
{
    return m_currentFile;
}

// Loads a file into the editor
bool CodeEditor::loadFile(const QString& filename) noexcept
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        Log::log("[Error] Cannot open file '" + filename + "' because of " + file.errorString());
        return false;
    }
    else
    {
        m_currentFile = filename;
        QTextStream in(&file);
        QString text = in.readAll();
        this->setPlainText(text);
        file.close();

        // Set the code editor read-only attribute to false
        setReadOnly(false);
        m_highlightError = true;

        return true;
    }
}

int CodeEditor::lineNumberAreaWidth() noexcept
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 16 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) noexcept
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) noexcept
{
    if (dy)
    {
        m_lineNumberArea->scroll(0, dy);
    }
    else
    {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}


void CodeEditor::resizeEvent(QResizeEvent *e) noexcept
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::selectLine(uint32_t line) noexcept
{
    // line-1 because line number starts from 0
    QTextCursor cursor(this->document()->findBlockByLineNumber(line-1));
    this->setTextCursor(cursor);

    // Center scroll bar on selected line
    uint32_t height = this->height();
    uint32_t rows = height / 19;
    uint32_t bottom = this->verticalScrollBar()->value();
    uint32_t top = bottom + rows;
    // Determine if if in top half or bottom half
    if (line > bottom + rows/2)
    {
        bottom += (rows/2 - (top - line));
    }
    else
    {
        bottom -= (rows/2 - (line - bottom));
    }

    this->verticalScrollBar()->setValue(bottom);
}


void CodeEditor::highlightCurrentLine() noexcept
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    // Only highlight if a file is loaded
    if (!isReadOnly() && m_currentFile.length())
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(m_lineNumberBackgroundColour);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        if (m_highlightError)
        {
            selection.format.setForeground(QColor(255,0,0));
            m_highlightError = false;
        }
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) noexcept
{
    QPainter painter(m_lineNumberArea.get());
    painter.fillRect(event->rect(), m_lineNumberAreaColour);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    // Only draw the line number if we have a loaded file
    while (block.isValid() && top <= event->rect().bottom() && m_currentFile.length())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height()+4,Qt::AlignLeft, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        blockNumber++;
    }
}

bool CodeEditor::eventFilter(QObject *object, QEvent *event) noexcept
{
    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if(wheel->modifiers() == Qt::ControlModifier)
        {
            // TODO: Fix broken zoom code
            static int zoom = 1;
            static int currentZoom = 100; // 100%
            const int zoomFactor = 2;
            if(wheel->delta() > 0)
            {
                currentZoom += zoomFactor;
                zoomIn(zoomFactor);
                zoom++;
            }
            else
            {
                if (currentZoom >= 92)
                {
                    currentZoom -= zoomFactor;
                    zoomOut(zoomFactor);
                    zoom--;
                }
            }
            m_zoomLabel->setText("Zoom: " + QString::number((int)(currentZoom)) + "%");
            return true;
        }
        else
        {
            return QObject::eventFilter(object, event);
        }
    }
    else
    {
        return QObject::eventFilter(object, event);
    }
}

void CodeEditor::setLabel(QLabel* label) noexcept
{
    m_zoomLabel = label;
}
