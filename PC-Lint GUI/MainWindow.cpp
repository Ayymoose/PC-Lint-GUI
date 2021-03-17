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
    m_linterStatus(0),
    m_lintTreeErrors(0),
    m_lintTreeWarnings(0),
    m_lintTreeInformation(0)
{
    qRegisterMetaType<PCLint::LintStatus>("Status");
    qRegisterMetaType<QSet<PCLint::LintMessage>>("QSet<LintMessage>");
    qRegisterMetaType<PCLint::LintData>("LintData");
    qRegisterMetaType<PCLint::LintResponse>("LintResponse");
    qRegisterMetaType<PCLint::LintData>("PCLint::LintData");
    qRegisterMetaType<PCLint::Version>("PCLint::Version");
    qRegisterMetaType<PCLint::LintMessageGroup>("LintMessageGroup");

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
    m_actionError->setText("Errors:" + QString::number(m_lintTreeErrors));
    m_buttonErrors->setDefaultAction(m_actionError.get());
    m_actionError->setCheckable(true);
    m_actionError->setChecked(m_toggleError);

    m_buttonWarnings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionWarning->setIcon(QIcon(":/images/warning.png"));
    m_actionWarning->setText("Warnings:" + QString::number(m_lintTreeWarnings));
    m_buttonWarnings->setDefaultAction(m_actionWarning.get());
    m_actionWarning->setCheckable(true);
    m_actionWarning->setChecked(m_toggleWarning);

    m_buttonInformation->setCheckable(true);
    m_buttonInformation->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionInformation->setIcon(QIcon(":/images/info.png"));
    m_actionInformation->setText("Information:" + QString::number(m_lintTreeInformation));
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
        applyLintTreeFilter();
    });

    QObject::connect(m_actionInformation.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleInformation = checked;
        applyLintTreeFilter();
    });

    QObject::connect(m_actionWarning.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleWarning = checked;
        applyLintTreeFilter();
    });

    QObject::connect(this, &MainWindow::signalUpdateTypes, this, [this]()
    {
        m_actionError->setText("Errors:" + QString::number(m_lintTreeErrors));
        m_actionWarning->setText("Warnings:" + QString::number(m_lintTreeWarnings));
        m_actionInformation->setText("Information:" + QString::number(m_lintTreeInformation));
    });

    m_ui->lintTable->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_ui->lintTable, &QTreeWidget::customContextMenuRequested, this, &MainWindow::handleContextMenu);

    // With syntax highlighting
    m_highlighter = std::make_unique<PCLint::Highlighter>(m_ui->codeEditor->document());

    // TreeWidget font set to 12
    m_ui->lintTable->setStyleSheet("QTreeWidget { font-size: 12pt; }");

    //testLintTreeFilter();
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

bool MainWindow::verifyLint()
{
    QDir path;
    QFileInfo fileInfo;

    // Check if executable exists
    QString linterExecutable = m_preferences->getLintExecutablePath().trimmed();
    fileInfo.setFile(linterExecutable);

    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "PC-Lint/PC-Lint Plus executable does not exist: '" + linterExecutable + "'");
        return false;
    }

    if (!fileInfo.isExecutable())
    {
        QMessageBox::critical(this,"Error", "Non-executable file specified: '" + linterExecutable + "'");
        return false;
    }

    // Check if lint file exists
    QString linterLintFile = m_preferences->getLintFilePath().trimmed();
    fileInfo.setFile(linterLintFile);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "PC-Lint file (.lnt) does not exist: '" + linterLintFile + "'");
        return false;
    }

    return true;
}

void MainWindow::on_actionLint_triggered()
{
    m_ui->lintTable->setSortingEnabled(false);
    //startLint(false);
}

/*
void MainWindow::addTreeMessageGroup(const PCLint::LintMessageGroup& lintMessageGroup) noexcept
{

    auto createTreeNode = [](QTreeWidgetItem* parentItem, const PCLint::LintMessage& message)
    {
        auto* treeItem = new QTreeWidgetItem(parentItem);
        treeItem->setText(PCLint::LINT_TABLE_FILE_COLUMN, QFileInfo(message.file).fileName());
        treeItem->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::UserRole, message.file);
        treeItem->setText(PCLint::LINT_TABLE_NUMBER_COLUMN, QString::number(message.number));
        treeItem->setText(PCLint::LINT_TABLE_DESCRIPTION_COLUMN, message.description);
        treeItem->setText(PCLint::LINT_TABLE_LINE_COLUMN, QString::number(message.line));

        auto const icon = PCLint::Lint::associateMessageTypeWithIcon(message.type);
        treeItem->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::DecorationRole, QPixmap::fromImage(icon));
        return treeItem;
    };

    for (const auto& groupMessage : lintMessageGroup)
    {
        // Every vector must be at least 1 otherwise something went wrong
        Q_ASSERT(groupMessage.size() >= 1);

        // Create the top-level item
        const auto messageTop = groupMessage.front();

        // Group together items under the same file
        auto const messageTopFileName = QFileInfo(messageTop.file).fileName();
        auto const treeList = m_ui->lintTable->findItems(messageTopFileName,Qt::MatchExactly, PCLint::LINT_TABLE_FILE_COLUMN);

        QTreeWidgetItem* fileDetailsItemTop = nullptr;

        if (treeList.size())
        {
            // Should only ever be 1 file name, unless there are mutiple files with the same name but different path?
            // Already exists, use this one
            Q_ASSERT(treeList.size() == 1);
            fileDetailsItemTop = treeList.first();
        }
        else
        {
            // New file entry
            fileDetailsItemTop = new QTreeWidgetItem(m_ui->lintTable);
            fileDetailsItemTop->setText(PCLint::LINT_TABLE_FILE_COLUMN, messageTopFileName);
            fileDetailsItemTop->setData(PCLint::LINT_TABLE_FILE_COLUMN, Qt::UserRole, messageTop.file);
        }

        // Filter
        if (filterMessageType(messageTop.type))
        {
            continue;
        }

        // If it's just a single entry
        if (groupMessage.size() == 1)
        {
            createTreeNode(fileDetailsItemTop, messageTop);
            // Skip next
            continue;
        }

        // Create a child level item
        auto fileDetailsItem = createTreeNode(fileDetailsItemTop, messageTop);

        Q_ASSERT(groupMessage.size() > 1);

        // Otherwise grab the rest of the group
        for (auto cit = groupMessage.cbegin()+1; cit != groupMessage.cend(); cit++)
        {
            auto message = *cit;

            // Filter
            // TODO: Should this be message.type?!
            if (filterMessageType(messageTop.type))
            {
                continue;
            }

            // Check if the file exists (absolute path given)
            // Check if it exists in the project file's directory
            if (!QFile(message.file).exists())
            {
                // TODO: Optimise this
                const auto relativeFile = QFileInfo(m_preferences->getLinterLintFilePath().trimmed()).canonicalPath() + '/' + message.file;
                if (!QFile(relativeFile).exists())
                {
                    message.file = "";
                }
                else
                {
                    message.file = relativeFile;
                }
            }
            // The sticky details
            createTreeNode(fileDetailsItem, message);
        }
    }


}
*/

void MainWindow::clearLintTree() noexcept
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

void MainWindow::applyLintTreeFilter() noexcept
{
    auto startLintTime = std::chrono::steady_clock::now();

    // Purge tree
    clearLintTree();

    auto endLintTime = std::chrono::steady_clock::now();
    auto totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug() << "applyLintTreeFilter/clearLintTree" << totalElapsedTime / 1000.f << "s elapsed";


    startLintTime = std::chrono::steady_clock::now();

    // Apply filter to tree
    // TODO: May be faster to apply filter to single type than check all 3
    //groupLintTreeMessages(m_lintTreeMessages);

    endLintTime = std::chrono::steady_clock::now();
    totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug() << "applyLintTreeFilter/groupLintTreeMessages" << totalElapsedTime / 1000.f << "s elapsed";

    startLintTime = std::chrono::steady_clock::now();

    // Clear any orphaned nodes
    QTreeWidgetItemIterator treeItr(m_ui->lintTable);
    while (*treeItr)
    {
        if ((*treeItr)->parent() == nullptr && (*treeItr)->childCount() == 0)
        {
            delete *treeItr;
        }
        else
        {
            treeItr++;
        }
    }

    endLintTime = std::chrono::steady_clock::now();
    totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug() << "applyLintTreeFilter/clearOrphanedNodes" << totalElapsedTime / 1000.f << "s elapsed";


    // Update message types
    emit signalUpdateTypes();

}

void MainWindow::testLintTreeFilter() noexcept
{
    QFile dataFile(R"(D:\Users\Ayman\Desktop\PC-Lint GUI\test\lintChunks.xml)");
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        Q_ASSERT(false);
    }
    auto lintChunk = dataFile.readAll();
    Q_ASSERT(lintChunk.size() > 0);
    dataFile.close();

    PCLint::Lint lint;
    auto lintResponse = lint.testLintProcessMessages(lintChunk);

    //slotProcessLintMessages(lintResponse);
}

void MainWindow::slotProcessLintMessageGroup(const PCLint::LintMessageGroup& lintMessageGroup) noexcept
{
    // Group messages together (PCLP only)

    auto const startLintTime = std::chrono::steady_clock::now();


    //addTreeMessageGroup(lintMessageGroup);

   // auto& lintMessages = lintResponse.lintMessages;

    // Group tree messages
    //groupLintTreeMessages(lintMessages);

    // Maintain tree messages for filtering
    /*m_lintTreeMessages.insert(m_lintTreeMessages.end(),lintMessages.begin(), lintMessages.end());

    m_lintTreeErrors += lintResponse.numberOfErrors;
    m_lintTreeWarnings += lintResponse.numberOfWarnings;
    m_lintTreeInformation += lintResponse.numberOfInformation;*/

    emit signalUpdateTypes();

    auto const endLintTime = std::chrono::steady_clock::now();
    auto const totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endLintTime - startLintTime).count();
    qDebug() << "MainWindow::slotProcessLintMessageGroup" << totalElapsedTime / 1000.f << "s elapsed";

}

void MainWindow::slotLintComplete(const PCLint::LintStatus& lintStatus, const QString& ) noexcept
{
    // Lint complete. Now update the table
    qInfo() << "Lint finished with" << lintStatus;

    // TODO: Fix this inconsistency between m_linter and these arguments
    // Show message if there are no lint problems
    /*if (lintStatus == PCLint::LintStatus::STATUS_COMPLETE &&
            m_linter.numberOfErrors() == 0 && m_linter.numberOfWarnings() == 0 && m_linter.numberOfInfo() == 0)
    {
        QMessageBox::information(this, "Information", "No errors detected in code");
    }
    else
    {
        // Display any outstanding messages
        if (lintStatus & PCLint::LintStatus::STATUS_PARTIAL_COMPLETE)
        {
            // TODO: This doesn't work correctly as sometimes we have 17/16 files shown
            QMessageBox::information(this, "Information", "Not all files were successfully linted as errors were generated in the lint output.");
        }
        else if (lintStatus & PCLint::LintStatus::STATUS_LICENSE_ERROR)
        {
            QMessageBox::critical(this, "Error", m_linter.errorMessage());
        }
        else if (lintStatus & PCLint::LintStatus::STATUS_PROCESS_ERROR)
        {
            QMessageBox::critical(this, "Error", "Failed to complete lint because of an internal error");
        }
        else if (lintStatus & PCLint::LintStatus::STATUS_PROCESS_TIMEOUT)
        {
            QMessageBox::critical(this, "Error", "Failed to complete lint because process became stuck");
        }
    }*/
    m_ui->lintTable->setSortingEnabled(true);

}

void MainWindow::startLint(QString)
{
    clearLintTree();
    m_lintTreeMessages.clear();

    ProgressWindow progressWindow;
    Qt::WindowFlags flags = progressWindow.windowFlags();
    progressWindow.setWindowFlags(flags | Qt::Tool);

    PCLint::Lint lint;
    lint.setLintExecutable(m_preferences->getLintExecutablePath().trimmed());
    lint.setLintFile(m_preferences->getLintFilePath().trimmed());


    QObject::connect(&lint, &PCLint::Lint::signalLintFinished, &progressWindow, &ProgressWindow::slotLintFinished);
    QObject::connect(&lint, &PCLint::Lint::signalLintComplete, &progressWindow, &ProgressWindow::slotLintComplete);
    QObject::connect(&progressWindow, &ProgressWindow::signalAbortLint, &lint, &PCLint::Lint::slotAbortLint);


    // Currently only supporting PC-Lint Plus
    QObject::connect(&lint, &PCLint::Lint::signalUpdateProgress, &progressWindow, &ProgressWindow::slotUpdateProgress);
    QObject::connect(&lint, &PCLint::Lint::signalUpdateProgressMax, &progressWindow, &ProgressWindow::slotUpdateProgressMax);
    QObject::connect(&lint, &PCLint::Lint::signalUpdateProcessedFiles, &progressWindow, &ProgressWindow::slotUpdateProcessedFiles);

    // Lint passes processed chunk to ProgressWindow
    QObject::connect(&lint, &PCLint::Lint::signalProcessLintMessageGroup, this, &MainWindow::slotProcessLintMessageGroup);

    QObject::connect(this, &MainWindow::signalPointerToLintTree, &lint, &PCLint::Lint::slotPointerToLintTree);


    // ProgressWindow asks MainWindow for lint data
    // MainWindow gives ProgressWindow lint data
    // ProgressWindow gives lint data to LintManager which starts the process
    // Start lint
    // Lint object gives LintManager lint responses
    // LintManager gives ProgressWindow progress data
    // ...
    // LintManager finishes and gives ProgressWindow the data
    // ProgressWindow gives MainWindow the output data

    // ProgressWindow asks MainWindow for lint data
    // MainWindow gives ProgressWindow lint data
    // ProgressWindow gives lint data to LintManager which starts the process
    // Start lint
    // LintManager gives ProgressWindow progress data
    // ...
    // ProgressWindow gives MainWindow the output data

    emit signalPointerToLintTree(m_ui->lintTable);
    lint.lint();

    progressWindow.exec();
}

void MainWindow::on_aboutLinty_triggered()
{
    m_about.display();
}

void MainWindow::on_actionRefresh_triggered()
{
    // If we have a project name that was already loaded
    // Then try to lint that again
    if (!m_lastProjectLoaded.isEmpty())
    {
       // m_linter.setLintFile(m_preferences->getLinterLintFilePath().trimmed());
        //m_linter.setLintExecutable(m_preferences->getLinterExecutablePath().trimmed());
        //m_linter.setSourceFiles(m_directoryFiles);
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

QSet<QString> MainWindow::recursiveBuildSourceFileSet(const QString& directory)
{
    qDebug() << "Scanning directory: " << directory;
    QSet<QString> directoryFiles;
    // Add any of these file types (e.g .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx.)

    const QStringList extensions[] = {{"*.c"},{"*.C"}, {"*.cc"}, {"*.cpp"}, {"*.CPP"}, {"*.c++"}, {"*.cp"}, {"*.cxx"}};
    for (const QStringList& stringList : extensions)
    {
        QDirIterator dirIterator(directory, stringList, QDir::Files, QDirIterator::Subdirectories);
        while (dirIterator.hasNext())
        {
            directoryFiles.insert(dirIterator.next());
        }
    }
    qDebug() << "Total files scanned: " << directoryFiles.size();
    return directoryFiles;
}

void MainWindow::on_actionLintProject_triggered()
{
   startLint("");
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
                QMessageBox::critical(this, "Error", "Unable to open file: " + fileToLoad);
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

/*
bool MainWindow::filterMessageType(const QString& type) const noexcept
{
    bool filter = false;

    if (!m_toggleError && (type == PCLint::Type::TYPE_ERROR))
    {
        filter = true;
    }
    else if (!m_toggleWarning && (type == PCLint::Type::TYPE_WARNING))
    {
        filter = true;
    }
    else if (!m_toggleInformation && (type == PCLint::Type::TYPE_INFO))
    {
        filter = true;
    }
    return filter;
}*/
