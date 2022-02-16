#include "worker.h"


Worker::Worker(QObject *parent, QMutex*m, QWaitCondition * w) : QObject(parent), mutex(m), waitMutex(w)
{
    finished = false;
}


void Worker::doWork()
{
    QMutexLocker locker(&selfmutex);

    while(true) {
        if(paused){
            waitMutex->wait(&selfmutex);         // QWaitCondition
        }
        int x = (qrand() % endline) + startline;
        startline = x+1;
        endline = endline+5;
        int y = (qrand() % 12);
        emit signalForPrint(x,y);
        QThread::msleep(1500);
        if(finished) {
            return;
        }
    }
}

void Worker::stop()
{
    finished = true;
}

Worker::~Worker()
{
    qDebug() << "worker destroyed";
}
