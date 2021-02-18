/********************************************************************************
** Form generated from reading UI file 'LintOptions.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINTOPTIONS_H
#define UI_LINTOPTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LintOptions
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *lintOptionsGroupBox;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_4;
    QLabel *lintPathLabel;
    QLabel *lintOptionsLabel;
    QPushButton *lintFileButton;
    QPushButton *lintButton;
    QLabel *sourceDirectoryLabel;
    QPushButton *sourceButton;
    QLineEdit *sourceFileInputText;
    QLineEdit *lintFileInputText;
    QLineEdit *lintOptionsInputText;
    QLabel *lintFileLntLabel;
    QLineEdit *lintInputText;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LintOptions)
    {
        if (LintOptions->objectName().isEmpty())
            LintOptions->setObjectName(QString::fromUtf8("LintOptions"));
        LintOptions->resize(555, 238);
        gridLayout_2 = new QGridLayout(LintOptions);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lintOptionsGroupBox = new QGroupBox(LintOptions);
        lintOptionsGroupBox->setObjectName(QString::fromUtf8("lintOptionsGroupBox"));
        verticalLayout = new QVBoxLayout(lintOptionsGroupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        lintPathLabel = new QLabel(lintOptionsGroupBox);
        lintPathLabel->setObjectName(QString::fromUtf8("lintPathLabel"));

        gridLayout_4->addWidget(lintPathLabel, 1, 0, 1, 1);

        lintOptionsLabel = new QLabel(lintOptionsGroupBox);
        lintOptionsLabel->setObjectName(QString::fromUtf8("lintOptionsLabel"));

        gridLayout_4->addWidget(lintOptionsLabel, 3, 0, 1, 1);

        lintFileButton = new QPushButton(lintOptionsGroupBox);
        lintFileButton->setObjectName(QString::fromUtf8("lintFileButton"));

        gridLayout_4->addWidget(lintFileButton, 2, 2, 1, 1);

        lintButton = new QPushButton(lintOptionsGroupBox);
        lintButton->setObjectName(QString::fromUtf8("lintButton"));

        gridLayout_4->addWidget(lintButton, 1, 2, 1, 1);

        sourceDirectoryLabel = new QLabel(lintOptionsGroupBox);
        sourceDirectoryLabel->setObjectName(QString::fromUtf8("sourceDirectoryLabel"));

        gridLayout_4->addWidget(sourceDirectoryLabel, 0, 0, 1, 1);

        sourceButton = new QPushButton(lintOptionsGroupBox);
        sourceButton->setObjectName(QString::fromUtf8("sourceButton"));

        gridLayout_4->addWidget(sourceButton, 0, 2, 1, 1);

        sourceFileInputText = new QLineEdit(lintOptionsGroupBox);
        sourceFileInputText->setObjectName(QString::fromUtf8("sourceFileInputText"));

        gridLayout_4->addWidget(sourceFileInputText, 0, 1, 1, 1);

        lintFileInputText = new QLineEdit(lintOptionsGroupBox);
        lintFileInputText->setObjectName(QString::fromUtf8("lintFileInputText"));

        gridLayout_4->addWidget(lintFileInputText, 2, 1, 1, 1);

        lintOptionsInputText = new QLineEdit(lintOptionsGroupBox);
        lintOptionsInputText->setObjectName(QString::fromUtf8("lintOptionsInputText"));

        gridLayout_4->addWidget(lintOptionsInputText, 3, 1, 1, 1);

        lintFileLntLabel = new QLabel(lintOptionsGroupBox);
        lintFileLntLabel->setObjectName(QString::fromUtf8("lintFileLntLabel"));

        gridLayout_4->addWidget(lintFileLntLabel, 2, 0, 1, 1);

        lintInputText = new QLineEdit(lintOptionsGroupBox);
        lintInputText->setObjectName(QString::fromUtf8("lintInputText"));

        gridLayout_4->addWidget(lintInputText, 1, 1, 1, 1);

        buttonBox = new QDialogButtonBox(lintOptionsGroupBox);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

        gridLayout_4->addWidget(buttonBox, 4, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_4);


        gridLayout_2->addWidget(lintOptionsGroupBox, 0, 0, 1, 1);


        retranslateUi(LintOptions);

        QMetaObject::connectSlotsByName(LintOptions);
    } // setupUi

    void retranslateUi(QDialog *LintOptions)
    {
        LintOptions->setWindowTitle(QCoreApplication::translate("LintOptions", "Lint Options", nullptr));
        lintOptionsGroupBox->setTitle(QCoreApplication::translate("LintOptions", "Lint options", nullptr));
        lintPathLabel->setText(QCoreApplication::translate("LintOptions", "Lint path:", nullptr));
        lintOptionsLabel->setText(QCoreApplication::translate("LintOptions", "Lint options:", nullptr));
        lintFileButton->setText(QCoreApplication::translate("LintOptions", "...", nullptr));
        lintButton->setText(QCoreApplication::translate("LintOptions", "...", nullptr));
        sourceDirectoryLabel->setText(QCoreApplication::translate("LintOptions", "Source directory (.c .h):", nullptr));
        sourceButton->setText(QCoreApplication::translate("LintOptions", "...", nullptr));
        lintFileLntLabel->setText(QCoreApplication::translate("LintOptions", "Lint file (.lnt):", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LintOptions: public Ui_LintOptions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINTOPTIONS_H
