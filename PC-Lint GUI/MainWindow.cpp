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

#include "Jenkins.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Linter.h"
#include "ProgressWindow.h"
#include "ProjectSolution.h"
#include "Compiler.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_lowerToolbar(std::make_unique<QToolBar>()),
    m_buttonErrors(std::make_unique<QToolButton>()),
    m_buttonWarnings(std::make_unique<QToolButton>()),
    m_buttonInfo(std::make_unique<QToolButton>()),
    m_actionError(std::make_unique<QAction>()),
    m_actionWarning(std::make_unique<QAction>()),
    m_actionInfo(std::make_unique<QAction>()),
    // Toggled on (show messages only of this type)
    // Toggle off (hide messages only of this type)
    m_toggleError(true),
    m_toggleWarning(true),
    m_toggleInfo(true),
    m_preferences(std::make_unique<Preferences>(this)),
    m_lintTableMenu(std::make_unique<QMenu>(this)),
    m_linterStatus(0),
    m_modifiedFileWorker(std::make_unique<ModifiedFileThread>(this))
{ 
    qRegisterMetaType<Lint::Status>("Lint::Status");
    qRegisterMetaType<QSet<LintMessage>>("QSet<LintMessage>");
    //qRegisterMetaType<QMap<QString,QDateTime>>("QMap<QString,QDateTime>");
    qRegisterMetaType<LintData>("LintData");
    qRegisterMetaType<LintResponse>("LintResponse");

    // Turn UI into actual objects
    m_ui->setupUi(this);

    QObject::connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    QObject::connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    QObject::connect(m_ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    QObject::connect(m_ui->actionCut, &QAction::triggered, this, &MainWindow::cut);

    // Configure the lint table
    // Icon column width
    m_ui->lintTable->setColumnWidth(0,24);

    // Code width
    m_ui->lintTable->setColumnWidth(1,40);

    // Message width
    m_ui->lintTable->setColumnWidth(2,800);

    m_ui->lintTable->setColumnWidth(3,200);

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
    m_actionError->setText("Errors: 0");
    m_buttonErrors->setDefaultAction(m_actionError.get());
    m_actionError->setCheckable(true);
    m_actionError->setChecked(m_toggleError);

    m_buttonWarnings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionWarning->setIcon(QIcon(":/images/warning.png"));
    m_actionWarning->setText("Warnings: 0");
    m_buttonWarnings->setDefaultAction(m_actionWarning.get());
    m_actionWarning->setCheckable(true);
    m_actionWarning->setChecked(m_toggleWarning);

    m_buttonInfo->setCheckable(true);
    m_buttonInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_actionInfo->setIcon(QIcon(":/images/info.png"));
    m_actionInfo->setText("Information: 0");
    m_buttonInfo->setDefaultAction(m_actionInfo.get());
    m_actionInfo->setCheckable(true);
    m_actionInfo->setChecked(m_buttonInfo.get());

    m_ui->verticalLayout_2->addWidget(m_lowerToolbar.get());
    m_lowerToolbar->addWidget(m_buttonErrors.get());
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonWarnings.get());
    m_lowerToolbar->addSeparator();
    m_lowerToolbar->addWidget(m_buttonInfo.get());
    m_lowerToolbar->addSeparator();

    QObject::connect(m_actionError.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleError = checked;
        displayLintTable();
    });

    QObject::connect(m_actionInfo.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleInfo = checked;
        displayLintTable();
    });

    QObject::connect(m_actionWarning.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleWarning = checked;
        displayLintTable();
    });

    QObject::connect(this, &MainWindow::signalUpdateTypes, this, [this](int errors, int warnings, int info)
    {
        m_actionError->setText("Errors: " + QString::number(errors));
        m_actionWarning->setText("Warnings: " + QString::number(warnings));
        m_actionInfo->setText("Information: " + QString::number(info));
    });

    m_ui->lintTable->setContextMenuPolicy(Qt::CustomContextMenu);
    //QObject::connect(m_ui->lintTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::handleContextMenu);

    // With syntax highlighting
    m_highlighter = std::make_unique<Highlighter>(m_ui->codeEditor->document());

    // Start the modified file thread
    QObject::connect(this, &MainWindow::signalSetModifiedFile, m_modifiedFileWorker.get(), &ModifiedFileThread::slotSetModifiedFile);
    QObject::connect(this, &MainWindow::signalSetModifiedFiles, m_modifiedFileWorker.get(), &ModifiedFileThread::slotSetModifiedFiles);
    QObject::connect(this, &MainWindow::signalKeepFile, m_modifiedFileWorker.get(), &ModifiedFileThread::slotKeepFile);
    QObject::connect(m_modifiedFileWorker.get(), &ModifiedFileThread::signalFileDoesntExist, this, &MainWindow::slotFileDoesntExist);
    QObject::connect(m_modifiedFileWorker.get(), &ModifiedFileThread::signalFileModified, this, &MainWindow::slotFileModified);
    m_modifiedFileWorker->start();

    // TreeWidget font set to 12
    m_ui->lintTable->setStyleSheet("QTreeWidget { font-size: 12pt; }");
}

MainWindow::~MainWindow()
{
    delete m_ui;
    // Stop all threads
    m_modifiedFileWorker->requestInterruption();
    m_modifiedFileWorker->quit();
    m_modifiedFileWorker->wait();
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

        emit signalSetModifiedFile(currentFile, QFileInfo(file).lastModified());

        m_ui->statusBar->showMessage("Saved " + currentFile + " at " + QDateTime::currentDateTime().toString());
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
    QString linterExecutable = m_preferences->getLinterExecutablePath().trimmed();
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
    QString linterLintFile = m_preferences->getLinterLintFilePath().trimmed();
    fileInfo.setFile(linterLintFile);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Error", "PC-Lint file (.lnt) does not exist: '" + linterLintFile + "'");
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

    // If a project is selected
    // If we have a lint file associated with it, start the linter on the project with the given lint file
    // Otherwise select the lint file, associate it with the project

    // Similar logic for directory


    if (verifyLint())
    {
        QSet<QString> directoryFiles;
        QString directoryToLint;
        QString fileName;

        // Lint a project solution file
        if (lintProject)
        {
            fileName = QFileDialog::getOpenFileName(this, "Select project file", Preferences::m_lastDirectory,
                                                    "Atmel 7 Studio (*.cproj);; "
                                                    "Visual Studio Project (*.vcxproj);; "
                                                    "Visual Studio Solution (*.sln) "
                                                    );
            if (!fileName.isEmpty())
            {

                // Have we got an association to a lint file already?

                // What if we want to use a different lint file for a project?
                // Where would we display the lint file being used?


                // Check file extension
                QString fileExtension = QFileInfo(fileName).completeSuffix();
                Preferences::m_lastDirectory = QFileInfo(fileName).absolutePath();

                ProjectSolution *project= nullptr;

                // Possible solutions/projects
                AtmelStudio7ProjectSolution as7ProjectSolution;
                VisualStudioProject vsProject;
                VisualStudioProjectSolution vsProjectSolution;

                if (fileExtension == "cproj")
                {
                    // Atmel Studio 7 project supported
                    project = &as7ProjectSolution;

                }
                else if (fileExtension == "vcxproj")
                {
                    // Visual studio project
                    project = &vsProject;
                }
                else if (fileExtension == "sln")
                {
                    // Visual studio solution
                    project = &vsProjectSolution;
                    project->setDirectory(QFileInfo(fileName).canonicalPath());
                }

                Q_ASSERT(project);

                try
                {
                    directoryFiles = project->buildSourceFiles(fileName);
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
            // Lint only C or CPP files
            directoryToLint = QFileDialog::getExistingDirectory(this,
                                                            tr("Select directory to lint"),
                                                            Preferences::m_lastDirectory,
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            if (!directoryToLint.isEmpty())
            {
                directoryFiles = recursiveBuildSourceFileSet(directoryToLint);
                m_directoryFiles = directoryFiles;
            }

        }
        //

        // Only start linting if a file was selected
        if (directoryFiles.size())
        {
            m_linter.setLinterFile(m_preferences->getLinterLintFilePath().trimmed());
            m_linter.setLinterExecutable(m_preferences->getLinterExecutablePath().trimmed());
            m_linter.setLintFiles(directoryFiles);

            // Display directory name or filename
            if (!fileName.isEmpty())
            {
                startLintThread(QFileInfo(fileName).fileName());
            }
            else
            {
                // Start linting a directory
                startLintThread(directoryToLint);
            }
        }
    }

}

void MainWindow::on_lintTable_cellDoubleClicked(int row, int)
{
    // Get the file
    // Column 3 is just the file name
    /*auto* item = m_ui->lintTable->item(row,3);
    Q_ASSERT(item);
    QString fileToLoad = item->data(Qt::UserRole).value<QString>();


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
                DEBUG_LOG("Loading file: " + fileToLoad);

                // Select the line number
                item = m_ui->lintTable->item(row,4);
                uint32_t lineNumber = item->text().toUInt();
                m_ui->codeEditor->selectLine(lineNumber);

                // Update the status bar
                m_ui->statusBar->showMessage("Loaded " + fileToLoad + " at " + QDateTime::currentDateTime().toString());
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
            item = m_ui->lintTable->item(row,4);
            uint32_t lineNumber = item->text().toUInt();
            m_ui->codeEditor->selectLine(lineNumber);
            qDebug() << fileToLoad << " already loaded";
        }



    }*/

}

void MainWindow::on_actionLint_triggered()
{
    startLint(false);
}

void MainWindow::slotLintFinished(const LintResponse& lintResponse)
{
    // Accumulate lint data here
    // Only when lint complete is sent then we populate lint table

    if (lintResponse.status == Lint::Status::LINTER_PARTIAL_COMPLETE ||
            lintResponse.status == Lint::Status::LINTER_COMPLETE ||
            lintResponse.status == Lint::Status::LINTER_PROCESS_ERROR)
    {
        m_linter.appendLinterMessages(lintResponse.lintMessages);
        m_linter.appendLinterErrors(lintResponse.numberOfErrors);
        m_linter.appendLinterWarnings(lintResponse.numberOfWarnings);
        m_linter.appendLinterInfo(lintResponse.numberOfInfo);
    }
    else
    {
        qDebug() << "[Error] Linter error " << lintResponse.status << "occurred so not appending any data";
        m_linter.setErrorMessage(lintResponse.errorMessage);
    }

    // Save status
    m_linterStatus |= lintResponse.status;
}

void MainWindow::displayLintTable()
{
    // Populate the table view with all the lint messages
    // Clear all existing entries

    QMap<QString, ModifiedFile> modifiedFiles;

    //m_ui->lintTable->setSortingEnabled(false);

    // Get ALL lint messages (combination from all threads)
    auto const groupedMessages = m_linter.groupLinterMessages();

    // Clear all entries
    auto treeWidget = m_ui->lintTable;
    while (treeWidget->topLevelItemCount())
    {
        QTreeWidgetItemIterator treeItr(treeWidget, QTreeWidgetItemIterator::NoChildren);
        while (*treeItr)
        {
            delete (*treeItr);
            treeItr++;
        }
    }

    //lintTable->clearContents();
    //lintTable->setRowCount(0);

    // Group messages together
    for (const auto& groupMessage : groupedMessages)
    {
        // Every vector must be at least 1 otherwise something went wrong in groupLinterMessages()
        Q_ASSERT(groupMessage.size() >= 1);

        // Create the top-level item
        const auto messageTop = groupMessage.front();

        auto* fileDetailsItemTop = new QTreeWidgetItem(m_ui->lintTable);
        fileDetailsItemTop->setText(0, QFileInfo(messageTop.file).fileName());

        // If it's just a single entry
        if (groupMessage.size() == 1)
        {
            auto* detailsItemTop = new QTreeWidgetItem(fileDetailsItemTop);
            detailsItemTop->setText(0, messageTop.type);
            detailsItemTop->setText(1, messageTop.number);
            detailsItemTop->setText(2, messageTop.description);
            detailsItemTop->setText(3, messageTop.line);

            // Skip next
            continue;
        }

        // Create the top-level item again
        auto* fileDetailsItem = new QTreeWidgetItem(fileDetailsItemTop);
        fileDetailsItem->setText(0, QFileInfo(messageTop.file).fileName());

        // Otherwise grab the rest of the group
        for (const auto& message : groupMessage)
        {
            const auto number = message.number;
            auto file = message.file;
            const auto line = message.line;
            const auto type = message.type;
            const auto description = message.description;

            // Check if the file exists (absolute path given)
            // Check if it exists in the project file's directory
            if (!QFile(file).exists())
            {
                const auto relativeFile = QFileInfo(m_linter.getLinterFile()).canonicalPath() + QDir::separator() + file;
                if (!QFile(relativeFile).exists())
                {
                    DEBUG_LOG("[Error] Unknown file in linter messages: " + file);
                    file = "";
                }
                else
                {
                    file = relativeFile;
                }
            }

            Lint::Message messageType;

            // Determine type
            if (!QString::compare(type, Lint::Type::LINT_TYPE_ERROR, Qt::CaseInsensitive))
            {
                messageType = Lint::Message::MESSAGE_TYPE_ERROR;
            }
            else if (!QString::compare(type, Lint::Type::LINT_TYPE_WARNING, Qt::CaseInsensitive))
            {
                messageType = Lint::Message::MESSAGE_TYPE_WARNING;
            }
            else if (!QString::compare(type, Lint::Type::LINT_TYPE_INFO, Qt::CaseInsensitive))
            {
                messageType = Lint::Message::MESSAGE_TYPE_INFORMATION;
            }
            else if (!QString::compare(type, Lint::Type::LINT_TYPE_SUPPLEMENTAL, Qt::CaseInsensitive))
            {
                messageType = Lint::Message::MESSAGE_TYPE_SUPPLEMENTAL;
            }
            else
            {
                messageType = Lint::Message::MESSAGE_TYPE_UNKNOWN;
            }

            // Filter
            if (!m_toggleError)
            {
                if (messageType == Lint::Message::MESSAGE_TYPE_ERROR)
                {
                    continue;
                }
            }

            if (!m_toggleWarning)
            {
                if (messageType == Lint::Message::MESSAGE_TYPE_WARNING)
                {
                    continue;
                }
            }

            if (!m_toggleInfo)
            {
                if (messageType == Lint::Message::MESSAGE_TYPE_INFORMATION || messageType == Lint::Message::MESSAGE_TYPE_SUPPLEMENTAL)
                {
                    continue;
                }
            }

            // The sticky details
            auto* detailsItem = new QTreeWidgetItem(fileDetailsItem);
            detailsItem->setText(0, message.type);
            detailsItem->setText(1, message.number);
            detailsItem->setText(2, message.description);
            detailsItem->setText(3, message.line);


            // Insert row
            //lintTable->insertRow(lintTable->rowCount());

            // Set item data
           /* auto* typeWidget = new QTableWidgetItem;
            auto* codeWidget = new QTableWidgetItem;
            auto* lineWidget = new QTableWidgetItem;
            auto* fileWidget = new QTableWidgetItem;

            codeWidget->setData(Qt::DisplayRole,number.toUInt());
            lineWidget->setData(Qt::DisplayRole,line.toUInt());
            fileWidget->setData(Qt::DisplayRole, QFileInfo(file).fileName());
            fileWidget->setData(Qt::UserRole, file);
    */
            // Add to set of modified files
            ModifiedFile modifiedFile;
           /* modifiedFile.lastModified = QFileInfo(file).lastModified();
            modifiedFile.keepFile = true;
            modifiedFiles[fileWidget->data(Qt::UserRole).value<QString>()] = modifiedFile;
    */
            QImage icon;
            switch (messageType)
            {
                case Lint::Message::MESSAGE_TYPE_ERROR: icon.load(":/images/error.png");  break;
                case Lint::Message::MESSAGE_TYPE_WARNING: icon.load(":/images/warning.png"); break;
                case Lint::Message::MESSAGE_TYPE_INFORMATION: icon.load(":/images/info.png"); break;
                // TODO: Get an icon for supplemental
                case Lint::Message::MESSAGE_TYPE_SUPPLEMENTAL: icon.load(":/images/info.png"); break;
                default: Q_ASSERT(false); break;
            }

            //typeWidget->setData(Qt::DecorationRole, QPixmap::fromImage(icon));

            //lintTable->setItem( lintTable->rowCount()-1, 0, typeWidget);
            //lintTable->setItem( lintTable->rowCount()-1, 1, codeWidget);
            //lintTable->setItem( lintTable->rowCount()-1, 2, new QTableWidgetItem(description));
            //lintTable->setItem( lintTable->rowCount()-1, 3, fileWidget);
            //lintTable->setItem( lintTable->rowCount()-1, 4, lineWidget);

        }

    }
   //m_ui->lintTable->setSortingEnabled(true);

    m_ui->lintTable->expandAll();

    // Show message if there are no lint problems
    if (m_linter.numberOfErrors() == 0 && m_linter.numberOfWarnings() == 0 && m_linter.numberOfInfo() == 0)
    {
        // Set item data
        // TODO: Fix memory leak
        //auto type = new QTableWidgetItem;
        //type->setData(Qt::DecorationRole, QPixmap::fromImage(*m_icons[ICON_CORRECT]));
        //lintTable->setItem( lintTable->rowCount()-1, 0, type);
        //lintTable->setItem( lintTable->rowCount()-1, 2, new QTableWidgetItem("No errors were detected"));
    }

    emit signalUpdateTypes(m_linter.numberOfErrors(), m_linter.numberOfWarnings(), m_linter.numberOfInfo());


    // Display any outstanding messages
    if (m_linterStatus & Lint::Status::LINTER_PARTIAL_COMPLETE)
    {
        QMessageBox::information(this, "Information", "Not all files were successfully linted as errors were generated in the lint output.");
    }
    else if (m_linterStatus & Lint::Status::LINTER_UNSUPPORTED_VERSION)
    {
        QMessageBox::critical(this, "Error", "Failed to start lint because of unknown PC-Lint/PC-Lint Plus version.");
    }
    else if (m_linterStatus & Lint::Status::LINTER_LICENSE_ERROR)
    {
        QMessageBox::critical(this, "Error", m_linter.errorMessage());
    }
    else if (m_linterStatus & Lint::Status::LINTER_PROCESS_ERROR)
    {
        QMessageBox::critical(this, "Error", "Failed to complete lint because of an internal error");
    }
    else if (m_linterStatus & Lint::Status::LINTER_PROCESS_TIMEOUT)
    {
        QMessageBox::critical(this, "Error", "Failed to complete lint because process became stuck");
    }

    m_linterStatus = 0;

    // Start the file monitor thread
    emit signalSetModifiedFiles(modifiedFiles);
    emit signalStartMonitor();
}

void MainWindow::slotLintComplete()
{
    // All lints are complete
    // Now update the table
    qDebug() << "All lints are complete. Updating table...";
    displayLintTable();
}

void MainWindow::slotGetLinterData()
{
    LintData lintData
    {
       m_preferences->getLinterExecutablePath().trimmed(),
       m_preferences->getLinterLintFilePath().trimmed(),
       m_directoryFiles
    };
    emit signalSetLinterData(lintData);
}

void MainWindow::startLintThread(QString title)
{
    // Clear any existing data
    m_linter.resetLinter();

    ProgressWindow progressWindow(this,title);
    Qt::WindowFlags flags = progressWindow.windowFlags();
    progressWindow.setWindowFlags(flags | Qt::Tool);
    progressWindow.exec();
}

// TODO: Move to About.cpp
void MainWindow::on_aboutLinty_triggered()
{
    QMessageBox versionMessageBox(this);
    versionMessageBox.setIcon(QMessageBox::Information);
    versionMessageBox.addButton("Copy to clipboard", QMessageBox::AcceptRole);
    versionMessageBox.setWindowTitle("Information");
    char buildCompiler[64];

    // Compiler name + Compiler version + 32/64-bit version
    sprintf(buildCompiler,"%s %s 32-bit\n", COMPILER_NAME, COMPILER_VERSION);

    QString applicationInfo =
            "Build Version: " BUILD_VERSION "\n"
            "Build Date: " BUILD_DATE "\n"
            "Build Commit: " BUILD_COMMIT "\n"
            "Build Compiler: " + QString(buildCompiler) +"\n";

    versionMessageBox.setText(applicationInfo);
    switch (versionMessageBox.exec())
    {
    case QMessageBox::AcceptRole:
        QApplication::clipboard()->setText(applicationInfo);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    // If we have a project name that was already loaded
    // Then try to lint that again
    if (!m_lastProjectLoaded.isEmpty())
    {
        m_linter.setLinterFile(m_preferences->getLinterLintFilePath().trimmed());
        m_linter.setLinterExecutable(m_preferences->getLinterExecutablePath().trimmed());
        m_linter.setLintFiles(m_directoryFiles);
        startLintThread(QFileInfo(m_lastProjectLoaded).fileName());
    }
}

void MainWindow::slotFileModified(QString modifiedFile)
{
    const QSignalBlocker blocker(m_modifiedFileWorker.get());
    QMessageBox msgBox(this);
    msgBox.setText(modifiedFile);
    msgBox.setInformativeText("This file has been modified by another program. \n Do you want to reload it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    if (msgBox.exec() == QMessageBox::Yes)
    {

        // Check if the file was deleted before we try to load it again
        if (QFile(modifiedFile).exists())
        {
            if (m_ui->codeEditor->loadedFile() == modifiedFile)
            {
                // Reload the file into the code editor if it's one open
                if (m_ui->codeEditor->loadFile(modifiedFile))
                {
                    // Reload
                    DEBUG_LOG("Reloading file: " + modifiedFile);

                    // Update the status bar
                    m_ui->statusBar->showMessage("Reloaded " + modifiedFile + " at " + QDateTime::currentDateTime().toString());
                }
                else
                {
                    // TODO: Reason for failure
                    QMessageBox::critical(this, "Error", "Unable to open file: " + modifiedFile);
                }
            }
        }
        else
        {
            // Ask if they want to keep this file
            slotFileDoesntExist(modifiedFile);
        }


    }
}

void MainWindow::slotFileDoesntExist(const QString& deletedFile)
{
    const QSignalBlocker blocker(m_modifiedFileWorker.get());
    QMessageBox msgBox(this);
    msgBox.setText("The file " + deletedFile + " doesn't exist anymore.");
    msgBox.setInformativeText("Keep this file in editor?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::No)
    {
        // Remove
        DEBUG_LOG("Removing file: " + deletedFile);

        // Remove this file from the code editor if it's the loaded one
        if (m_ui->codeEditor->loadedFile() == deletedFile)
        {
            m_ui->codeEditor->clear();
        }

        // Update the linter messages
        m_linter.removeAssociatedMessages(deletedFile);

        // Update the lint table again
        displayLintTable();

        // Update the status bar
        m_ui->statusBar->showMessage("Removed " + deletedFile + " at " + QDateTime::currentDateTime().toString());

        // Notify the ModifiedWorker thread
        emit signalRemoveFile(deletedFile);
    }
    else
    {
        // Notify monitor thread that we want to keep this file
        // Save this file locall
        emit signalKeepFile(deletedFile);
    }
}

void MainWindow::handleContextMenu(const QPoint& pos)
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
    log.startDetached("notepad.exe", QStringList() << LOG_FILENAME);
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
    startLint(true);
}
