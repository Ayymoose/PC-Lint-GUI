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

#include <QPlainTextEdit>
#include <QColor>
#include <QLabel>


#define LINE_NUMBER_AREA_COLOUR QColor(240,240,240)
#define LINE_CURRENT_BACKGROUND_COLOUR QColor(252,249,241)

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();
    void lineNumberAreaPaintEvent(QPaintEvent *event) noexcept;
    int lineNumberAreaWidth() noexcept;
    bool loadFile(const QString& file) noexcept;
    void selectLine(uint32_t line) noexcept;
    void setLineNumberAreaColour(const QColor& colour) noexcept;
    void setLineNumberBackgroundColour(const QColor& colour) noexcept;
    QString loadedFile() const noexcept;
    void setLabel(QLabel* label) noexcept;
protected:
    void resizeEvent(QResizeEvent *event) noexcept override;
    bool eventFilter(QObject *object, QEvent *event) noexcept override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount) noexcept;
    void highlightCurrentLine() noexcept;
    void updateLineNumberArea(const QRect &rect, int dy) noexcept;

private:
    std::unique_ptr<LineNumberArea> m_lineNumberArea;
    QColor m_lineNumberAreaColour;
    QColor m_lineNumberBackgroundColour;
    QString m_currentFile;
    QLabel* m_zoomLabel;
    bool m_highlightError;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor),
        m_codeEditor(editor)
    {

    }

    QSize sizeHint() const override
    {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_codeEditor;
};
