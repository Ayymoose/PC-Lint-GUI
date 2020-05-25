#ifndef MODIFIEDFILETHREAD_H
#define MODIFIEDFILETHREAD_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QPair>

struct ModifiedFile
{
    QDateTime lastModified;
    bool keepFile;
};

class ModifiedFileThread : public QThread
{
    Q_OBJECT
public:
    explicit ModifiedFileThread(QObject *parent = nullptr) : QThread(parent) {}
    ~ModifiedFileThread();
protected:
    void run() override;
public slots:
    void slotSetModifiedFiles(QMap<QString, ModifiedFile> modifiedFiles);
    void slotSetModifiedFile(const QString& modifiedFile, const QDateTime& dateTime);
    void slotRemoveFile(const QString& deletedFile);
    void slotKeepFile(const QString& keepFile);

signals:
    void signalFinished();
    void signalFileModified(const QString& modifiedFile);
    void signalFileDoesntExist(const QString& deletedFile);
private:
    QMap<QString, ModifiedFile> m_modifiedFiles;
    QMutex m_modifiedFileMutex;
};

#endif // MODIFIEDFILETHREAD_H
