#include "LintOptions.h"
#include "ui_LintOptions.h"

LintOptions::LintOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LintOptions)
{
    ui->setupUi(this);
}

LintOptions::~LintOptions()
{
    delete ui;
}
