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

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QTimer>
#include <QThread>
#include <QToolButton>
#include <QProcess>
#include <QtGlobal>
#include <QClipboard>
#include <QTreeWidget>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Lint.h"
#include "ProgressWindow.h"
#include "ProjectSolution.h"
#include "About.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_lowerToolbar(std::make_unique<QToolBar>()),
    m_buttonErrors(std::make_unique<QToolButton>()),
    m_buttonWarnings(std::make_unique<QToolButton>()),
    m_buttonInformation(std::make_unique<QToolButton>()),
    m_actionError(std::make_unique<QAction>()),
    m_actionWarning(std::make_unique<QAction>()),
    m_actionInformation(std::make_unique<QAction>()),
    // Toggled on (show messages only of this type)
    // Toggle off (hide messages only of this type)
    m_toggleError(true),
    m_toggleWarning(true),
    m_toggleInformation(true),
    m_preferences(std::make_unique<Preferences>(this)),
    m_lintTableMenu(std::make_unique<QMenu>(this)),
    m_numberOfErrors(0),
    m_numberOfWarnings(0),
    m_numberOfInformations(0)
{
    qRegisterMetaType<PCLint::LintStatus>("Status");
    qRegisterMetaType<PCLint::LintMessageGroup>("LintMessageGroup");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // Turn UI into actual objects
    m_ui->setupUi(this);

    QObject::connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);

    // Configure the lint table
    // Icon column width
    m_ui->lintTable->setColumnWidth(PCLint::LINT_TABLE_FILE_COLUMN,256);
    m_ui->lintTable->setColumnWidth(PCLint::LINT_TABLE_NUMBER_COLUMN,80);
    m_ui->lintTable->setColumnWidth(PCLint::LINT_TABLE_DESCRIPTION_COLUMN,800);
    m_ui->lintTable->setColumnWidth(PCLint::LINT_TABLE_LINE_COLUMN,80);

    // Configure the code editor
    m_ui->codeEditor->setLineNumberAreaColour(LINE_NUMBER_AREA_COLOUR);
    m_ui->codeEditor->setLineNumberBackgroundColour(LINE_CURRENT_BACKGROUND_COLOUR);

    // Set the splitter size
    m_ui->splitter->setSizes(QList<int>() << 400 << 200);


    // Status bar labels
    m_ui->statusBar->addPermanentWidget(m_ui->label);
    m_ui->codeEditor->setLabel(m_ui->label);

    // | Errors: 0 Warnings: 0 Info: 0 |

    m_buttonErrors->setCheckable(true);
    m_buttonErrors->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionError->setIcon(QIcon(":/images/error.png"));
    m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
    m_buttonErrors->setDefaultAction(m_actionError.get());
    m_actionError->setCheckable(true);
    m_actionError->setChecked(m_toggleError);

    m_buttonWarnings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionWarning->setIcon(QIcon(":/images/warning.png"));
    m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
    m_buttonWarnings->setDefaultAction(m_actionWarning.get());
    m_actionWarning->setCheckable(true);
    m_actionWarning->setChecked(m_toggleWarning);

    m_buttonInformation->setCheckable(true);
    m_buttonInformation->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionInformation->setIcon(QIcon(":/images/info.png"));
    m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));
    m_buttonInformation->setDefaultAction(m_actionInformation.get());
    m_actionInformation->setCheckable(true);
    m_actionInformation->setChecked(m_buttonInformation.get());

    m_ui->verticalLayout_2->addWidget(m_lowerToolbar.get());
    m_lowerToolbar->addWidget(m_buttonErrors.get());
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonWarnings.get());
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonInformation.get());
    m_lowerToolbar->addSeparator();

    QObject::connect(m_actionError.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleError = checked;
        applyTreeFilter(m_toggleError, PCLint::Type::TYPE_ERROR);
    });

    QObject::connect(m_actionInformation.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleInformation = checked;
        applyTreeFilter(m_toggleInformation, PCLint::Type::TYPE_INFO);
    });

    QObject::connect(m_actionWarning.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleWarning = checked;
        applyTreeFilter(m_toggleWarning, PCLint::Type::TYPE_WARNING);
    });

    QObject::connect(this, &MainWindow::signalUpdateTypes, this, [this]()
    {
        m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
        m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
        m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));
    });

    m_ui->lintTable->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_ui->lintTable, &QTreeWidget::customContextMenuRequested, this, &MainWindow::handleContextMenu);

    // With syntax highlighting
    m_highlighter = std::make_unique<PCLint::Highlighter>(m_ui->codeEditor->document());

    // TreeWidget font set to 12
    m_ui->lintTable->setStyleSheet("QTreeWidget { font-size: 12pt; }");

}

MainWindow::~MainWindow()
{
    delete m_ui;
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
        m_ui->statusBar->showMessage("Saved " + currentFile + " at " + QDateTime::currentDateTime().toString());
    }
}

// Open preferences
void MainWindow::on_actionPreferences_triggered()
{
    m_preferences->setModal(true);
    m_preferences->exec();
}

bool MainWindow::checkLint()
{
    QFileInfo fileInfo;

    // Check if executable exists
    auto const lintExecutable = m_preferences->getLintExecutablePath().trimmed();
    if (lintExecutable.isEmpty())
    {
        QMessageBox::critical(this,"Error", "No PC-Lint/PC-Lint Plus executable specified in Preferences");
        return false;
    }
    fileInfo.setFile(lintExecutable);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "PC-Lint/PC-Lint Plus executable does not exist: '" + lintExecutable + "'");
        return false;
    }

    if (!fileInfo.isExecutable())
    {
        QMessageBox::critical(this,"Error", "Non-executable file specified: '" + lintExecutable + "'");
        return false;
    }

    // Check if lint file exists
    auto const lintFile = m_preferences->getLintFilePath().trimmed();
    if (lintFile.isEmpty())
    {
        QMessageBox::critical(this,"Error", "No lint file (.lnt) specified in Preferences");
        return false;
    }
    fileInfo.setFile(lintFile);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "Lint file (.lnt) does not exist: '" + lintFile + "'");
        return false;
    }

    return true;
}

void MainWindow::clearTreeNodes() const noexcept
{
    // Clear all entries
    while (m_ui->lintTable->topLevelItemCount())
    {
        QTreeWidgetItemIterator treeItr(m_ui->lintTable, QTreeWidgetItemIterator::NoChildren);
        while (*treeItr)
        {
            delete *treeItr;
        }
    }
}

void MainWindow::applyTreeFilter(bool filter, const QString& type) const noexcept
{
    QTreeWidgetItemIterator treeItr(m_ui->lintTable);
    while (*treeItr)
    {
        auto const treeNode = (*treeItr);
        // TODO: Optimise and use enum type instead of string
        auto const data = treeNode->data(PCLint::LINT_TABLE_DESCRIPTION_COLUMN, Qt::UserRole).value<QString>();
        if (filter && (data == type))
        {
            treeNode->setHidden(false);
        }
        else if (!filter && (data == type))
        {
            treeNode->setHidden(true);
        }
        treeItr++;
    }
}

void MainWindow::slotLintComplete(const PCLint::LintStatus& lintStatus, const QString& errorMessage) noexcept
{
    // Lint complete. Now update the table
    qInfo() << "Lint finished with" << lintStatus;

    switch (lintStatus)
    {
    case PCLint::LintStatus::STATUS_COMPLETE:
        // TODO: Need to get number of warnings, information and errors
        break;
    case PCLint::LintStatus::STATUS_PARTIAL_COMPLETE:
        QMessageBox::information(this, "Information", "Not all files were successfully linted as errors were generated in the lint output.");
        break;
    case PCLint::LintStatus::STATUS_LICENSE_ERROR:
        QMessageBox::critical(this, "Error", errorMessage);
        break;
    case PCLint::LintStatus::STATUS_PROCESS_ERROR:
        QMessageBox::critical(this, "Error", "Lint failed because of an interal process error:\n\n" + errorMessage);
        break;
    case PCLint::LintStatus::STATUS_PROCESS_TIMEOUT:
        QMessageBox::critical(this, "Error", "Lint failed due to being stuck linting a file for too long");
        break;
    case PCLint::LintStatus::STATUS_ABORT:
        // Lint aborted
    break;
    case PCLint::LintStatus::STATUS_UNKNOWN:
        // Unknown lint status
        Q_ASSERT(false);
    break;
    }
    m_ui->lintTable->setSortingEnabled(true);

}

void MainWindow::startLint(QString)
{
    m_numberOfErrors = 0;
    m_numberOfWarnings = 0;
    m_numberOfInformations = 0;

    m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
    m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
    m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));

    clearTreeNodes();

    m_progressWindow = std::make_unique<ProgressWindow>(this);
    m_lint = std::make_unique<PCLint::Lint>(m_preferences->getLintExecutablePath().trimmed(), m_preferences->getLintFilePath().trimmed());

    m_lint->setHardwareThreads(m_preferences->getLintHardwareThreads());

    QObject::connect(m_progressWindow.get(), &ProgressWindow::signalLintComplete, this, &MainWindow::slotLintComplete);
    QObject::connect(m_lint.get(), &PCLint::Lint::signalLintComplete, m_progressWindow.get(), &ProgressWindow::slotLintComplete);
    QObject::connect(m_progressWindow.get(), &ProgressWindow::signalAbortLint, m_lint.get(), &PCLint::Lint::slotAbortLint);

    // Currently only supporting PC-Lint Plus
    QObject::connect(m_lint.get(), &PCLint::Lint::signalUpdateProgress, m_progressWindow.get(), &ProgressWindow::slotUpdateProgress);
    QObject::connect(m_lint.get(), &PCLint::Lint::signalUpdateProgressMax, m_progressWindow.get(), &ProgressWindow::slotUpdateProgressMax);
    QObject::connect(m_lint.get(), &PCLint::Lint::signalUpdateProcessedFiles, m_progressWindow.get(), &ProgressWindow::slotUpdateProcessedFiles);

    QObject::connect(m_lint.get(), &PCLint::Lint::signalAddTreeMessageGroup, this, &MainWindow::addTreeMessageGroup);

    m_lint->lint();
    m_progressWindow->exec();
}

bool MainWindow::filterMessageType(const QString& type) const noexcept
{
    bool filter = false;
    if (!m_toggleInformation && (type == PCLint::Type::TYPE_INFO))
    {
        filter = true;
    }
    else if (!m_toggleError && (type == PCLint::Type::TYPE_ERROR))
    {
        filter = true;
    }
    else if (!m_toggleWarning && (type == PCLint::Type::TYPE_WARNING))
    {
        filter = true;
    }
    return filter;
}

// TODO: Optimise this function is very slow
void MainWindow::addTreeMessageGroup(const PCLint::LintMessageGroup& lintMessageGroup) noexcept
{

    auto addFullFilePath = [this](const QString& file)
    {
        // Check if the file exists (absolute path given)
        if (QFileInfo(file).exists())
        {
            return file;
        }

        // Check if it exists relative to the lint file
        auto const lintFilePath = QFileInfo(m_lint->getLintFile()).canonicalPath() + '/' + file;
        auto const canonFilePath = QFileInfo(lintFilePath).canonicalFilePath();

        // If canonical file path doesn't exist, it means the path we constructed failed and it returned ""
        if (!QFile(canonFilePath).exists())
        {
            if (!file.isEmpty())
            {
                //qInfo() << "Unknown file:" << file;
            }
            return QString();
        }
        else
        {
           return canonFilePath;
        }
    };

    // Create a new node in the lint tree table
    auto createTreeNode = [this, addFullFilePath](QTreeWidgetItem* parentItem, const PCLint::LintMessage& message)
    {
        auto* treeItem = new QTreeWidgetItem(parentItem);
        treeItem->setText(PCLint::LINT_TABLE_FILE_COLUMN, QFileInfo(message.file).fileName());
        treeItem->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::UserRole, addFullFilePath(message.file));
        treeItem->setText(PCLint::LINT_TABLE_NUMBER_COLUMN, QString::number(message.number));
        treeItem->setText(PCLint::LINT_TABLE_DESCRIPTION_COLUMN, message.description);
        treeItem->setText(PCLint::LINT_TABLE_LINE_COLUMN, QString::number(message.line));

        QImage icon;
        PCLint::Message messageType = PCLint::MESSAGE_UNKNOWN;

        if (message.type == PCLint::Type::TYPE_ERROR)
        {
            icon.load(":/images/error.png");
            m_numberOfErrors++;
            m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
            messageType = PCLint::MESSAGE_ERROR;
        }
        else if (message.type == PCLint::Type::TYPE_WARNING)
        {
            icon.load(":/images/warning.png");
            m_numberOfWarnings++;
            m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
            messageType = PCLint::MESSAGE_WARNING;
        }
        else if (message.type == PCLint::Type::TYPE_INFO)
        {
            icon.load(":/images/info.png");
            m_numberOfInformations++;
            m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));
            messageType = PCLint::MESSAGE_INFORMATION;
        }
        else if (message.type == PCLint::Type::TYPE_SUPPLEMENTAL)
        {
            icon.load(":/images/info.png");
            messageType = PCLint::MESSAGE_SUPPLEMENTAL;
        }
        else
        {
            Q_ASSERT(false);
        }

        treeItem->setData(PCLint::LINT_TABLE_DESCRIPTION_COLUMN, Qt::UserRole, message.type);
        treeItem->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::DecorationRole, QPixmap::fromImage(icon));

        // Filter if needed
        if (filterMessageType(message.type))
        {
            treeItem->setHidden(true);
        }

        return treeItem;
    };

    for (const auto& messageGroup : lintMessageGroup)
    {
        // Every vector must be at least 1 otherwise something went wrong
        Q_ASSERT(messageGroup.size() >= 1);

        // Create the top-level item
        const auto messageTop = messageGroup.front();

        // Group together items under the same file
        auto const messageTopFileName = QFileInfo(messageTop.file).fileName();
        auto const treeList = m_ui->lintTable->findItems(messageTopFileName,Qt::MatchExactly, PCLint::LINT_TABLE_FILE_COLUMN);

        QTreeWidgetItem* fileDetailsItemTop;

        if (treeList.size())
        {
            // TODO: Test with duplicate file names under different paths
            // Should only ever be 1 file name, unless there are mutiple files with the same name but different path?
            // Already exists, use this one
            Q_ASSERT(treeList.size() == 1);
            fileDetailsItemTop = treeList.first();
        }
        else
        {
            // New top level file entry
            fileDetailsItemTop = new QTreeWidgetItem(m_ui->lintTable);
            fileDetailsItemTop->setText(PCLint::LINT_TABLE_FILE_COLUMN, messageTopFileName);
            fileDetailsItemTop->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::UserRole, addFullFilePath(messageTop.file));
        }

        // If it's just a single entry
        if (messageGroup.size() == 1)
        {
            createTreeNode(fileDetailsItemTop, messageTop);
            // Skip next
            continue;
        }

        // Create a child level item
        auto fileDetailsItem = createTreeNode(fileDetailsItemTop, messageTop);

        Q_ASSERT(messageGroup.size() > 1);

        // Otherwise grab the rest of the group
        for (auto cit = messageGroup.cbegin()+1; cit != messageGroup.cend(); cit++)
        {
            // The sticky details
            createTreeNode(fileDetailsItem, *cit);
        }
    }
}

void MainWindow::on_aboutLint_triggered()
{
    m_about.display();
}

void MainWindow::on_actionRefresh_triggered()
{
    // If we have a project name that was already loaded
    // Then try to lint that again
    if (!m_lastProjectLoaded.isEmpty())
    {
        startLint(QFileInfo(m_lastProjectLoaded).fileName());
    }
}

void MainWindow::handleContextMenu(const QPoint& )
{
  /*  auto *item = m_ui->lintTable->itemAt(pos);

    if (item)
    {
        // Get the associated widgets
        auto* codeWidget = m_ui->lintTable->item(item->row(), 1);
        auto* fileWidget = m_ui->lintTable->item(item->row(), 3);

        m_lintTableMenu->clear();
        m_lintTableMenu->popup(m_ui->lintTable->horizontalHeader()->viewport()->mapToGlobal(pos));

        QAction* actionRemoveFile = m_lintTableMenu->addAction("Remove file from messages");
        QAction* actionHideMessages = m_lintTableMenu->addAction("Hide messages of this type");
        // QAction* actionSurpressMessages = m_lintTableMenu->addAction("Surpress this message in lint file");

        QObject::connect(actionRemoveFile, &QAction::triggered, this, [=]()
        {
            QString file = fileWidget->data(Qt::UserRole).value<QString>();
            m_linter.removeAssociatedMessages(file);
            displayLintTable();
        });

        QObject::connect(actionHideMessages, &QAction::triggered, this, [=]()
        {
            QString code = codeWidget->data(Qt::DisplayRole).value<QString>();
            m_linter.removeMessagesWithNumber(code);
            displayLintTable();
        });*/

        /*QObject::connect(actionSurpressMessages, &QAction::triggered, this, [=]()
        {
            // TODO: Write to lint file the surpression
            //auto s = fileWidget->data(Qt::DisplayRole).value<QString>();
            //DEBUG_LOG("Surpressing messages: " + QString(s));
        });*/
    //}
}

void MainWindow::on_actionLog_triggered()
{
    // Display the event log
    QProcess log;
    if (!log.startDetached("notepad.exe", QStringList() << PCLint::LOG_FILENAME))
    {
        QMessageBox::critical(this, "Error", log.errorString());
    }
}

void MainWindow::on_actionLint_triggered()
{
    if (checkLint())
    {
        startLint("");
    }
}

void MainWindow::on_lintTable_itemClicked(QTreeWidgetItem *item, int)
{
    // Get the file to load
    QString fileToLoad = item->data(PCLint::LINT_TABLE_FILE_COLUMN,Qt::UserRole).value<QString>();

    if (!fileToLoad.isEmpty())
    {
        // TODO:
        // If the file doesn't exist and we wanted to keep it in the editor, create it by saving the current loaded file
        // But check that the current loaded file is the one we are saving otherwise
        // How do save files that don't exist anymore?
        //if (!QFile(fileToLoad).exists())
        //{
        //    save();
        //}

        // Load the file into the code editor
        if (m_ui->codeEditor->loadedFile() != fileToLoad)
        {
            if (m_ui->codeEditor->loadFile(fileToLoad))
            {
                qInfo() << "Loading file: " << fileToLoad;

                // Select the line number
                QString lineNumber = item->data(PCLint::LINT_TABLE_LINE_COLUMN, Qt::DisplayRole).value<QString>();
                if (!lineNumber.isEmpty())
                {
                    m_ui->codeEditor->selectLine(lineNumber.toUInt());

                    // Update the status bar
                    m_ui->statusBar->showMessage("Loaded " + fileToLoad + " at " + QDateTime::currentDateTime().toString());
                }
            }
            else
            {
                // TODO: Reason for failure
                QMessageBox::critical(this, "Error", "1 Unable to open file: " + fileToLoad);
            }
        }
        else
        {
            // Select the line number
            QString lineNumber = item->data(PCLint::LINT_TABLE_LINE_COLUMN, Qt::DisplayRole).value<QString>();
            if (!lineNumber.isEmpty())
            {
                m_ui->codeEditor->selectLine(lineNumber.toUInt());
            }
        }
    }
}

