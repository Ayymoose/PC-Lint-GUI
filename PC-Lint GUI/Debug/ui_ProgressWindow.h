/********************************************************************************
** Form generated from reading UI file 'ProgressWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSWINDOW_H
#define UI_PROGRESSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProgressWindow
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *lintGroupBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLabel *labelTimeElapsed;
    QLabel *timeElapsed;
    QLabel *labelETA;
    QLabel *eta;
    QLabel *labelFilesProcessed;
    QLabel *filesProcessed;
    QProgressBar *lintProgressBar;
    QPushButton *lintCancel;

    void setupUi(QDialog *ProgressWindow)
    {
        if (ProgressWindow->objectName().isEmpty())
            ProgressWindow->setObjectName(QString::fromUtf8("ProgressWindow"));
        ProgressWindow->resize(318, 208);
        verticalLayout_2 = new QVBoxLayout(ProgressWindow);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lintGroupBox = new QGroupBox(ProgressWindow);
        lintGroupBox->setObjectName(QString::fromUtf8("lintGroupBox"));
        lintGroupBox->setMaximumSize(QSize(300, 16777215));
        verticalLayout_3 = new QVBoxLayout(lintGroupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelTimeElapsed = new QLabel(lintGroupBox);
        labelTimeElapsed->setObjectName(QString::fromUtf8("labelTimeElapsed"));

        gridLayout->addWidget(labelTimeElapsed, 0, 0, 1, 1);

        timeElapsed = new QLabel(lintGroupBox);
        timeElapsed->setObjectName(QString::fromUtf8("timeElapsed"));

        gridLayout->addWidget(timeElapsed, 0, 1, 1, 1);

        labelETA = new QLabel(lintGroupBox);
        labelETA->setObjectName(QString::fromUtf8("labelETA"));

        gridLayout->addWidget(labelETA, 1, 0, 1, 1);

        eta = new QLabel(lintGroupBox);
        eta->setObjectName(QString::fromUtf8("eta"));

        gridLayout->addWidget(eta, 1, 1, 1, 1);

        labelFilesProcessed = new QLabel(lintGroupBox);
        labelFilesProcessed->setObjectName(QString::fromUtf8("labelFilesProcessed"));

        gridLayout->addWidget(labelFilesProcessed, 2, 0, 1, 1);

        filesProcessed = new QLabel(lintGroupBox);
        filesProcessed->setObjectName(QString::fromUtf8("filesProcessed"));

        gridLayout->addWidget(filesProcessed, 2, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        lintProgressBar = new QProgressBar(lintGroupBox);
        lintProgressBar->setObjectName(QString::fromUtf8("lintProgressBar"));
        lintProgressBar->setValue(24);
        lintProgressBar->setTextVisible(false);

        verticalLayout_3->addWidget(lintProgressBar);

        lintCancel = new QPushButton(lintGroupBox);
        lintCancel->setObjectName(QString::fromUtf8("lintCancel"));

        verticalLayout_3->addWidget(lintCancel);


        verticalLayout_2->addWidget(lintGroupBox);


        retranslateUi(ProgressWindow);

        QMetaObject::connectSlotsByName(ProgressWindow);
    } // setupUi

    void retranslateUi(QDialog *ProgressWindow)
    {
        ProgressWindow->setWindowTitle(QCoreApplication::translate("ProgressWindow", "Form", nullptr));
        lintGroupBox->setTitle(QCoreApplication::translate("ProgressWindow", "ProjectName", nullptr));
        labelTimeElapsed->setText(QCoreApplication::translate("ProgressWindow", "Time elapsed:", nullptr));
        timeElapsed->setText(QCoreApplication::translate("ProgressWindow", "00:00:00", nullptr));
        labelETA->setText(QCoreApplication::translate("ProgressWindow", "Estimated time remaining:", nullptr));
        eta->setText(QCoreApplication::translate("ProgressWindow", "N/A", nullptr));
        labelFilesProcessed->setText(QCoreApplication::translate("ProgressWindow", "File:", nullptr));
        filesProcessed->setText(QCoreApplication::translate("ProgressWindow", "N/A", nullptr));
        lintProgressBar->setFormat(QString());
        lintCancel->setText(QCoreApplication::translate("ProgressWindow", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProgressWindow: public Ui_ProgressWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSWINDOW_H
