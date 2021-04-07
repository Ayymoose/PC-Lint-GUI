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
#include "PCLintPlus.h"
#include "ProgressWindow.h"
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
    m_m_lintTreeMenu(std::make_unique<QMenu>(this)),
    m_numberOfErrors(0),
    m_numberOfWarnings(0),
    m_numberOfInformations(0),
    m_parent(nullptr)
{
    qRegisterMetaType<Lint::Status>("Status");
    qRegisterMetaType<Lint::LintMessageGroup>("LintMessageGroup");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<Lint::LintMessage>("LintMessage");

    // Turn UI into actual objects
    m_ui->setupUi(this);

    QObject::connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::save);

    // Setup tree view
    setupLintTree();

    // Configure the code editor
    m_ui->m_codeEditor->setLineNumberAreaColour(LINE_NUMBER_AREA_COLOUR);
    m_ui->m_codeEditor->setLineNumberBackgroundColour(LINE_CURRENT_BACKGROUND_COLOUR);

    // Set the splitter size
    m_ui->splitter->setSizes(QList<int>() << 400 << 200);


    // Status bar labels
    m_ui->statusBar->addPermanentWidget(m_ui->label);
    m_ui->m_codeEditor->setLabel(m_ui->label);

    // | Errors: 0 Warnings: 0 Info: 0 |

    m_buttonErrors->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_buttonErrors->setDefaultAction(m_actionError.get());

    m_actionError->setIcon(QIcon(":/images/error.png"));
    m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
    m_actionError->setCheckable(true);
    m_actionError->setChecked(m_toggleError);

    m_buttonWarnings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_buttonWarnings->setDefaultAction(m_actionWarning.get());

    m_actionWarning->setIcon(QIcon(":/images/warning.png"));
    m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
    m_actionWarning->setCheckable(true);
    m_actionWarning->setChecked(m_toggleWarning);

    m_buttonInformation->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_buttonInformation->setDefaultAction(m_actionInformation.get());

    m_actionInformation->setIcon(QIcon(":/images/info.png"));
    m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));
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
        m_proxyModel.invalidate();
        m_proxyModel.setFilter(m_toggleError, m_toggleWarning, m_toggleInformation);
    });

    QObject::connect(m_actionInformation.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleInformation = checked;
        m_proxyModel.invalidate();
        m_proxyModel.setFilter(m_toggleError, m_toggleWarning, m_toggleInformation);
    });

    QObject::connect(m_actionWarning.get(), &QAction::triggered, this, [this](bool checked)
    {
        m_toggleWarning = checked;
        m_proxyModel.invalidate();
        m_proxyModel.setFilter(m_toggleError, m_toggleWarning, m_toggleInformation);
    });

    // With syntax highlighting
    m_highlighter = std::make_unique<Lint::Highlighter>(m_ui->m_codeEditor->document());
}

void MainWindow::setupLintTree() noexcept
{
    m_treeModel.setHorizontalHeaderItem(Lint::LINT_TABLE_FILE_COLUMN, new QStandardItem("File"));
    m_treeModel.setHorizontalHeaderItem(Lint::LINT_TABLE_NUMBER_COLUMN, new QStandardItem("Number"));
    m_treeModel.setHorizontalHeaderItem(Lint::LINT_TABLE_DESCRIPTION_COLUMN, new QStandardItem("Description"));
    m_treeModel.setHorizontalHeaderItem(Lint::LINT_TABLE_LINE_COLUMN, new QStandardItem("Line"));

    // TODO: Replace with QAbstractItemModel for performance
    m_proxyModel.setSourceModel(&m_treeModel);
    m_proxyModel.setFilter(m_toggleError, m_toggleWarning, m_toggleInformation);
    m_ui->m_lintTree->setModel(&m_proxyModel);

    m_ui->m_lintTree->setColumnWidth(Lint::LINT_TABLE_FILE_COLUMN,256);
    m_ui->m_lintTree->setColumnWidth(Lint::LINT_TABLE_NUMBER_COLUMN,80);
    m_ui->m_lintTree->setColumnWidth(Lint::LINT_TABLE_DESCRIPTION_COLUMN,800);
    m_ui->m_lintTree->setColumnWidth(Lint::LINT_TABLE_LINE_COLUMN,80);
}

auto MainWindow::createTreeNodes(const Lint::LintMessage& message) noexcept
{
    QList<QStandardItem*> items;

    auto* treeFileItem = new QStandardItem(QFileInfo(message.file).fileName());
    treeFileItem->setData(message.file, Qt::UserRole);  
    auto* treeNumberItem = new QStandardItem(QString::number(message.number));
    auto* treeDescriptionItem = new QStandardItem(message.description);
    treeDescriptionItem->setData(message.type, Qt::UserRole);
    auto* treeLineItem = new QStandardItem(QString::number(message.line));

    QImage icon;
    // TODO: Use enum Message instead of string for performance so this can be put in the LintMessage struct
    //Lint::Message messageType = Lint::MESSAGE_UNKNOWN;

    if (message.type == Lint::Type::TYPE_ERROR)
    {
        icon.load(":/images/error.png");
        m_numberOfErrors++;
        m_actionError->setText("Errors:" + QString::number(m_numberOfErrors));
        //messageType = PCLint::MESSAGE_ERROR;
    }
    else if (message.type == Lint::Type::TYPE_WARNING)
    {
        icon.load(":/images/warning.png");
        m_numberOfWarnings++;
        m_actionWarning->setText("Warnings:" + QString::number(m_numberOfWarnings));
        //messageType = PCLint::MESSAGE_WARNING;
    }
    else if (message.type == Lint::Type::TYPE_INFORMATION)
    {
        icon.load(":/images/info.png");
        m_numberOfInformations++;
        m_actionInformation->setText("Information:" + QString::number(m_numberOfInformations));
        //messageType = PCLint::MESSAGE_INFORMATION;
    }
    else if (message.type == Lint::Type::TYPE_SUPPLEMENTAL)
    {
        icon.load(":/images/info.png");
        //messageType = PCLint::MESSAGE_SUPPLEMENTAL;
    }
    else if (message.type == Lint::Type::TYPE_NOTE)
    {
        icon.load(":/images/info.png");
    }
    else
    {
        Q_ASSERT(false);
    }

    treeFileItem->setData(QPixmap::fromImage(icon), Qt::DecorationRole);

    items.append(treeFileItem);
    items.append(treeNumberItem);
    items.append(treeDescriptionItem);
    items.append(treeLineItem);

    return items;

};


// This should be in a separate class to handle this logic (TreeModel)
void MainWindow::slotAddTreeParent(const Lint::LintMessage& parentMessage) noexcept
{
    auto const parentName = QFileInfo(parentMessage.file).fileName();
    auto const parentList = m_treeModel.findItems(parentName, Qt::MatchExactly);
    QStandardItem* parentNode;

    // Determine if the we added the file to the tree already
    if (parentList.size() > 0)
    {
        Q_ASSERT(parentList.size() == 1);
        parentNode = parentList.front();
    }
    else
    {
        parentNode = new QStandardItem(parentName);
        // TODO: Why is the QModelIndex the QString here?
        parentNode->setData(parentMessage.file, Qt::UserRole);
        m_treeModel.appendRow(parentNode);
    }

    // Duplicate the parent node under itself
    auto treeItems = createTreeNodes(parentMessage);
    parentNode->appendRow(treeItems);

    // Set parent to first child
    m_parent = treeItems.first();
}

void MainWindow::slotAddTreeChild(const Lint::LintMessage& childMessage) noexcept
{
    // This slot must use the correct parent node
    Q_ASSERT(m_parent);

    // Add children under parent
    auto treeItems = createTreeNodes(childMessage);
    m_parent->appendRow(treeItems);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::save()
{
    QString currentFile = m_ui->m_codeEditor->loadedFile();

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
        QString text = m_ui->m_codeEditor->toPlainText();
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

void MainWindow::clearTreeNodes() noexcept
{
    m_treeModel.clear();
    setupLintTree();
    m_parent = nullptr;
}

void MainWindow::slotLintComplete(const Lint::Status& lintStatus, const QString& errorMessage) noexcept
{
    // Lint complete. Now update the table
    qInfo() << "Lint finished with" << lintStatus;

    switch (lintStatus)
    {
    case Lint::Status::STATUS_COMPLETE:
        // All is good
        break;
    case Lint::Status::STATUS_PARTIAL_COMPLETE:
        QMessageBox::information(this, "Information", "Not all files were successfully linted as errors were generated in the lint output.");
        break;
    case Lint::Status::STATUS_LICENSE_ERROR:
        QMessageBox::critical(this, "Error", errorMessage);
        break;
    case Lint::Status::STATUS_PROCESS_ERROR:
        QMessageBox::critical(this, "Error", "Lint failed because of an interal process error:\n\n" + errorMessage);
        break;
    case Lint::Status::STATUS_PROCESS_TIMEOUT:
        QMessageBox::critical(this, "Error", "Lint failed due to being stuck linting a file for too long");
        break;
    case Lint::Status::STATUS_ABORT:
        // Lint aborted
    break;
    case Lint::Status::STATUS_UNKNOWN:
        // Unknown lint status
        Q_ASSERT(false);
    break;
    }
    m_ui->m_lintTree->setSortingEnabled(true);

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
    m_lint = std::make_unique<Lint::PCLintPlus>(m_preferences->getLintExecutablePath().trimmed(), m_preferences->getLintFilePath().trimmed());

    m_lint->setHardwareThreads(m_preferences->getLintHardwareThreads());

    QObject::connect(m_progressWindow.get(), &ProgressWindow::signalLintComplete, this, &MainWindow::slotLintComplete);
    QObject::connect(m_lint.get(), &Lint::PCLintPlus::signalLintComplete, m_progressWindow.get(), &ProgressWindow::slotLintComplete);
    QObject::connect(m_progressWindow.get(), &ProgressWindow::signalAbortLint, m_lint.get(), &Lint::PCLintPlus::slotAbortLint);

    // Currently only supporting PC-Lint Plus
    QObject::connect(m_lint.get(), &Lint::PCLintPlus::signalUpdateProgress, m_progressWindow.get(), &ProgressWindow::slotUpdateProgress);
    QObject::connect(m_lint.get(), &Lint::PCLintPlus::signalUpdateProgressMax, m_progressWindow.get(), &ProgressWindow::slotUpdateProgressMax);

    QObject::connect(m_lint.get(), &Lint::PCLintPlus::signalAddTreeParent, this, &MainWindow::slotAddTreeParent);
    QObject::connect(m_lint.get(), &Lint::PCLintPlus::signalAddTreeChild, this, &MainWindow::slotAddTreeChild);

    m_progressWindow->setTitle(m_lint->getLintFile());
    m_progressWindow->show();
    m_lint->lint();
    m_progressWindow->setModal(true);

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

void MainWindow::on_actionLog_triggered()
{
    // Display the event log
    QProcess log;
    if (!log.startDetached("notepad.exe", QStringList() << Lint::LOG_FILENAME))
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


void MainWindow::on_m_lintTree_clicked(const QModelIndex&)
{
    QModelIndexList selection = m_ui->m_lintTree->selectionModel()->selectedIndexes();
    // 4 columns only
    Q_ASSERT(selection.size() == 4);

    // Get the file to load and line number
    auto const fileToLoad = selection[0].data(Qt::UserRole).value<QString>();
    auto const lineNumber = selection[3].data(Qt::DisplayRole).value<QString>();

    qDebug() << fileToLoad;

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
        if (m_ui->m_codeEditor->loadedFile() != fileToLoad)
        {
            if (m_ui->m_codeEditor->loadFile(fileToLoad))
            {
                qInfo() << "Loading file: " << fileToLoad;

                // Select the line number
                if (!lineNumber.isEmpty())
                {
                    m_ui->m_codeEditor->selectLine(lineNumber.toUInt());
                }
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
            if (!lineNumber.isEmpty())
            {
                m_ui->m_codeEditor->selectLine(lineNumber.toUInt());
            }
        }
    }
}
