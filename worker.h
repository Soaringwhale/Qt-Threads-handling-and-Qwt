#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr, QMutex *m = nullptr, QWaitCondition *w = nullptr);
    ~Worker();
    void doWork();
    void stop();

    bool paused = false;

signals:
    void signalForPrint(int,int);

private:
    QMutex *mutex;
    QMutex selfmutex;
    QWaitCondition * waitMutex;

    bool finished;
    int startline = 1, endline = 8;
private slots:

};

#endif // WORKER_H
