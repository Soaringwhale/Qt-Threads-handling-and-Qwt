#include "workercreator.h"


WorkerCreator::WorkerCreator(QMutex*m,  QWaitCondition * w) : cr_mutex(m), cr_waitMutex(w)
{
    worker = nullptr;
}

WorkerCreator::~WorkerCreator ()
{
    if (worker != NULL) {
        delete worker;
    }
}

void WorkerCreator::createWorker()
{
    worker  = new Worker(0, cr_mutex, cr_waitMutex);
    connect(worker, SIGNAL(signalForPrint(int,int)), this, SLOT(receiverSlot(int,int)), Qt::DirectConnection);

    worker->doWork();

    emit finished();
    return ;
}


void WorkerCreator::creatorStopSlot() {
    if(worker != nullptr) {
        worker->stop();
    }
    return ;
}

void WorkerCreator::receiverSlot(int x, int y)
{
   emit signal1(x,y);
}


void WorkerCreator::cr_slot_pause()
{
    worker->paused = true;
}

void WorkerCreator::cr_slot_resume()
{
    worker->paused = false;
}
