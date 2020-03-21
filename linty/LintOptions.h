#ifndef LINTOPTIONS_H
#define LINTOPTIONS_H

#include <QDialog>

namespace Ui {
class LintOptions;
}

class LintOptions : public QDialog
{
    Q_OBJECT

public:
    explicit LintOptions(QWidget *parent = nullptr);
    ~LintOptions();

private:
    Ui::LintOptions *ui;
};

#endif // LINTOPTIONS_H
