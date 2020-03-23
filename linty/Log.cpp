#include "Log.h"

//Log::Log(QObject *parent, QString fileName) : QObject(parent) What is the effect of calling the parent

std::unique_ptr<QFile> Log::m_file;

void Log::createLogFile(const QString &file)
{
    m_file = std::make_unique<QFile>(new QFile);
    m_file->setFileName(file);
    m_file->open(QIODevice::Append | QIODevice::Text);
}

void Log::log(const QString &value)
{
    QString text = value;
    text = QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss] ") + text + "\n";
    QTextStream out(m_file.get());
    out.setCodec("UTF-8");
    if (m_file != nullptr)
    {
        out << text;
    }
}
