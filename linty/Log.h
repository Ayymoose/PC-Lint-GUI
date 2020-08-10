#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <memory>
#include <QtDebug>

#define DEBUG_LOG(MESSAGE) qDebug() << (MESSAGE);/* Log::log(MESSAGE);*/
#define LOG_FILENAME "linty_log.txt"

class Log : public QObject
{
    Q_OBJECT
public:
    static void createLogFile(const QString &file) noexcept;
    static void log(const QString &message) noexcept;
    Log() = delete;
private:
    static std::unique_ptr<QFile> m_file;
};

#endif // LOGGER_H
