/********************************************************************************
** Form generated from reading UI file 'Preferences.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCES_H
#define UI_PREFERENCES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Preferences
{
public:
    QWidget *formLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *lintPathExeLabel;
    QLineEdit *lintFileLineEdit;
    QPushButton *lintPathFileOpen;
    QLineEdit *lintPathExeLineEdit;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *lintUsingThreadsComboBox;
    QLabel *labelMaxHardwareThreads;
    QSpacerItem *horizontalSpacer;
    QLabel *labelMaxThreads;
    QPushButton *lintFileFileOpen;
    QSpacerItem *verticalSpacer;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_2;
    QPushButton *buttonSave;
    QPushButton *buttonCancel;
    QLabel *label_3;
    QTreeWidget *preferencesTree;

    void setupUi(QDialog *Preferences)
    {
        if (Preferences->objectName().isEmpty())
            Preferences->setObjectName(QString::fromUtf8("Preferences"));
        Preferences->resize(580, 430);
        Preferences->setMaximumSize(QSize(580, 430));
        formLayoutWidget = new QWidget(Preferences);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(160, 40, 411, 108));
        gridLayout = new QGridLayout(formLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        lintPathExeLabel = new QLabel(formLayoutWidget);
        lintPathExeLabel->setObjectName(QString::fromUtf8("lintPathExeLabel"));

        gridLayout->addWidget(lintPathExeLabel, 1, 0, 1, 1);

        lintFileLineEdit = new QLineEdit(formLayoutWidget);
        lintFileLineEdit->setObjectName(QString::fromUtf8("lintFileLineEdit"));

        gridLayout->addWidget(lintFileLineEdit, 2, 2, 1, 1);

        lintPathFileOpen = new QPushButton(formLayoutWidget);
        lintPathFileOpen->setObjectName(QString::fromUtf8("lintPathFileOpen"));
        lintPathFileOpen->setMaximumSize(QSize(32, 16777215));

        gridLayout->addWidget(lintPathFileOpen, 1, 3, 1, 1);

        lintPathExeLineEdit = new QLineEdit(formLayoutWidget);
        lintPathExeLineEdit->setObjectName(QString::fromUtf8("lintPathExeLineEdit"));

        gridLayout->addWidget(lintPathExeLineEdit, 1, 2, 1, 1);

        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lintUsingThreadsComboBox = new QComboBox(formLayoutWidget);
        lintUsingThreadsComboBox->setObjectName(QString::fromUtf8("lintUsingThreadsComboBox"));
        lintUsingThreadsComboBox->setMaximumSize(QSize(32, 16777215));

        horizontalLayout_2->addWidget(lintUsingThreadsComboBox);

        labelMaxHardwareThreads = new QLabel(formLayoutWidget);
        labelMaxHardwareThreads->setObjectName(QString::fromUtf8("labelMaxHardwareThreads"));

        horizontalLayout_2->addWidget(labelMaxHardwareThreads);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_2, 3, 2, 1, 1);

        labelMaxThreads = new QLabel(formLayoutWidget);
        labelMaxThreads->setObjectName(QString::fromUtf8("labelMaxThreads"));

        gridLayout->addWidget(labelMaxThreads, 3, 0, 1, 1);

        lintFileFileOpen = new QPushButton(formLayoutWidget);
        lintFileFileOpen->setObjectName(QString::fromUtf8("lintFileFileOpen"));
        lintFileFileOpen->setMaximumSize(QSize(32, 16777215));

        gridLayout->addWidget(lintFileFileOpen, 2, 3, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 2, 1, 1);

        gridLayoutWidget = new QWidget(Preferences);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(390, 390, 181, 41));
        gridLayout_2 = new QGridLayout(gridLayoutWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        buttonSave = new QPushButton(gridLayoutWidget);
        buttonSave->setObjectName(QString::fromUtf8("buttonSave"));

        gridLayout_2->addWidget(buttonSave, 0, 1, 1, 1);

        buttonCancel = new QPushButton(gridLayoutWidget);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        gridLayout_2->addWidget(buttonCancel, 0, 2, 1, 1);

        label_3 = new QLabel(Preferences);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(160, 20, 161, 20));
        preferencesTree = new QTreeWidget(Preferences);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("PC-Lint/PC-Lint Plus"));
        preferencesTree->setHeaderItem(__qtreewidgetitem);
        preferencesTree->setObjectName(QString::fromUtf8("preferencesTree"));
        preferencesTree->setGeometry(QRect(10, 10, 141, 411));

        retranslateUi(Preferences);

        QMetaObject::connectSlotsByName(Preferences);
    } // setupUi

    void retranslateUi(QDialog *Preferences)
    {
        Preferences->setWindowTitle(QCoreApplication::translate("Preferences", "Preferences", nullptr));
        lintPathExeLabel->setText(QCoreApplication::translate("Preferences", "Lint path (.exe)", nullptr));
        lintPathFileOpen->setText(QCoreApplication::translate("Preferences", "...", nullptr));
        label->setText(QCoreApplication::translate("Preferences", "Lint file (.lnt)", nullptr));
        labelMaxHardwareThreads->setText(QCoreApplication::translate("Preferences", "/ ", nullptr));
        labelMaxThreads->setText(QCoreApplication::translate("Preferences", "Max threads:", nullptr));
        lintFileFileOpen->setText(QCoreApplication::translate("Preferences", "...", nullptr));
        buttonSave->setText(QCoreApplication::translate("Preferences", "Save", nullptr));
        buttonCancel->setText(QCoreApplication::translate("Preferences", "Cancel", nullptr));
        label_3->setText(QCoreApplication::translate("Preferences", "<html><head/><body><p><span style=\" font-weight:600;\">PC-Lint/PC-Lint Plus options</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Preferences: public Ui_Preferences {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCES_H
