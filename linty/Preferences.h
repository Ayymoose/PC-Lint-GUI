#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui
{
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
    QString getLinterExecutablePath() const noexcept;
    QString getLinterLintFilePath() const noexcept;
    static QString m_lastDirectory;

private slots:
    void on_lintPathFileOpen_clicked();
    void on_lintFileFileOpen_clicked();
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();

private:
    Ui::Preferences* m_ui;
};
