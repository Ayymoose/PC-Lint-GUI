#include "Settings.h"
#include <QSettings>
#include <QDebug>

QMap<QString,QString> Settings::loadINISettings()
{
    // Load settings from INI
    QMap<QString,QString> keyValues;
    QSettings settings(INI_SETTINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(INI_SETTINGS_GROUP);
    QStringList keys = settings.allKeys();
    foreach (const QString &key, keys)
    {
        QString value = settings.value(key).toString();
        keyValues.insert(key,value);
    }
    settings.endGroup();
    return keyValues;
}

void Settings::writeINI(const QString &key, const QVariant &value)
{
    QSettings settings(INI_SETTINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(INI_SETTINGS_GROUP);
    settings.setValue(key,value);
    settings.endGroup();

    qDebug() << "[Settings::writeINI] Writing to " << settings.fileName();
    qDebug() << settings.status();
}
