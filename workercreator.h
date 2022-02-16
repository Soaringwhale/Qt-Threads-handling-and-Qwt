#ifndef WORKERCREATOR_H
#define WORKERCREATOR_H

#include <QObject>
#include <QString>
#include "worker.h"
#include <QMutex>

class WorkerCreator : public QObject
{
    Q_OBJECT
private:
    Worker *worker;

    QMutex *cr_mutex;
    QWaitCondition * cr_waitMutex;

public:
    WorkerCreator(QMutex*m, QWaitCondition * w);
    ~WorkerCreator();

public slots:
    void createWorker();
    void receiverSlot(int, int);
    void creatorStopSlot();

    void cr_slot_pause();
    void cr_slot_resume();

signals:
    void finished();
    void signal1(int, int);
};




#endif // WORKERCREATOR_H
