#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <memory>

#define LOG_FILENAME "linty_log.txt"

class Log : public QObject
{
    Q_OBJECT
public:
    static void createLogFile(const QString &file);
    static void log(const QString &message);
    Log() = delete;
private:
    static std::unique_ptr<QFile> m_file;
};

#endif // LOGGER_H
