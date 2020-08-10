#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QVariant>

#define INI_SETTINGS_FILE "linty_settings"
#define INI_SETTINGS_GROUP "LINT_OPTIONS"

class Settings
{

public:
    static QMap<QString,QString> loadINISettings() noexcept;
    static void writeINI(const QString &key, const QVariant &value) noexcept;
private:
    Settings() = delete;
};

#endif // SETTINGS_H
