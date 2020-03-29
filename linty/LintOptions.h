#ifndef LINTOPTIONS_H
#define LINTOPTIONS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class LintOptions;
}

class LintOptions : public QDialog
{
    Q_OBJECT

public:
    explicit LintOptions(QWidget *parent = nullptr);
    ~LintOptions();

    // Common class needed for this maybe
    void loadSettings();

    QString getLinterExecutablePath() const;
    QString getLinterLintFilePath() const;
    QString getLinterLintOptions() const;
    QString getLinterDirectory() const;
    static QString m_lastDirectory;

private slots:

    void on_sourceButton_clicked();

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_lintButton_clicked();

    void on_lintFileButton_clicked();

private:
    Ui::LintOptions *m_ui;

};

#endif // LINTOPTIONS_H
