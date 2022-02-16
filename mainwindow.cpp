#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isThreadExist = false;
    isPausedNow = false;
    isStoppedNow = true;
    isMainStopped = false;

    map = new QMap<int,int>;            //  not used in this version
    plot = new QwtPlot(this);
    hlayout1 = new QHBoxLayout(this);
    hlayout2 = new QHBoxLayout(this);
    vlayout = new QVBoxLayout(this);

    hlayout2->addWidget(plot);

    QPixmap img1(":/play.png");
    QPixmap img2(":/pause.png");
    QPixmap img3(":/stop.png");

    labelStart = new QLabel(this);
    labelPause = new QLabel(this);
    labelStop = new QLabel(this);
    labelText1 = new QLabel(this);
    labelText2 = new QLabel(this);
    labelText3 = new QLabel(this);

    btnStartMain = new QPushButton("Pause Main",this);
    btnPauseMain = new QPushButton("Start Main",this);
    btnStopMain = new QPushButton("Stop Main",this);

    labelStart->installEventFilter(this);
    labelPause->installEventFilter(this);
    labelStop->installEventFilter(this);

    labelStart->resize(44, 44);
    labelPause->resize(44,44);
    labelStop->resize(44,44);

    labelStart->setPixmap(img1.scaled(labelStart->size(), Qt::KeepAspectRatio));
    labelPause->setPixmap(img2.scaled(labelStart->size()));
    labelStop->setPixmap(img3.scaled(labelStart->size()));
    labelText1->setText("start/resume thread ");
    labelText2->setText("pause thread ");
    labelText3->setText("finish thread ");

    hlayout1->addSpacing(60);
    hlayout1->addWidget(labelText1);
    hlayout1->addWidget(labelStart);
    hlayout1->addWidget(labelText2);
    hlayout1->addWidget(labelPause);
    hlayout1->addWidget(labelText3);
    hlayout1->addWidget(labelStop);
    hlayout1->addSpacing(115);
    hlayout1->addWidget(btnPauseMain);
    hlayout1->addWidget(btnStartMain);
    hlayout1->addWidget(btnStopMain);

    hlayout1->addSpacing(90);

     vlayout->addLayout(hlayout2);
     vlayout->addLayout(hlayout1);
     this->centralWidget()->setLayout(vlayout);

       setFixedSize(950,650);
       plot->resize(800,500);

           plot->setTitle( "Qwt Thread Project" );
           plot->setCanvasBackground( Qt::white );

           plot->setAxisTitle(QwtPlot::yLeft, "Y");
           plot->setAxisTitle(QwtPlot::xBottom, "X");
           plot->insertLegend( new QwtLegend() );

           curve = new QwtPlotCurve();
           curve->setTitle( "Thread2" );
           curve->setPen( Qt::red, 1 );

           curve2 = new QwtPlotCurve();
           curve2->setTitle( "MainThread" );
           curve2->setPen( Qt::blue, 1 );


           QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,  QBrush( Qt::red ), QPen( Qt::blue, 2 ), QSize( 8, 8 ) );

           curve->setSymbol( symbol );
           curve2->setSymbol( symbol );

              QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas());
              magnifier->setMouseButton(Qt::MidButton);

              QwtPlotPanner *d_panner = new QwtPlotPanner(plot->canvas() );
              d_panner->setMouseButton( Qt::RightButton );

              curve->attach(plot);
              curve2->attach(plot);

              timer1 = new QTimer(this);

              connect(btnStartMain, SIGNAL(clicked()), this, SLOT(pauseMainLine()));
              connect(btnPauseMain, SIGNAL(clicked()), this, SLOT(resumeMainLine()));
              connect(btnStopMain, SIGNAL(clicked()), this, SLOT(stopMainLine()));
              connect(timer1, SIGNAL(timeout()), this, SLOT(timerSlotForMainThread()));
              timer1->start(1500);


}


bool MainWindow::startClicked()
{
   if(!isThreadExist)
   {
       isStoppedNow = false;
       isThreadExist = true;
       points.clear();
       addThread();                      // начало отрисовки красного графика с нуля
       curve->attach(plot);
       plot->replot();
       return true;
   }
   if(isPausedNow) {
       isStoppedNow = false;
       isPausedNow = false;                            
        emit mainSignalResume();
        mWait.wakeOne();               // продолжение отрисовки из состояния паузы
        return true;
   }
   return false;
}

bool MainWindow::pauseClicked()
{
     qDebug() << "paused";
     isPausedNow = true;
     emit mainSignalPause();          // pause
}

bool MainWindow::stopClicked()
{
      qDebug() << "stopping";

      if(isPausedNow){
          isPausedNow = false;
      }
      mWait.wakeOne();
      QThread::msleep(300);
      emit stopAll();                   // stop
      QThread::msleep(300);
      isThreadExist = false;
      map->clear();
      curve->detach();
      plot->replot();
      isStoppedNow = true;
      return true;
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == labelStart && event->type() == QEvent::MouseButtonPress) {
        if(!isThreadExist || isPausedNow) {
              return(startClicked());                                                 //  label start event
        }
    }

    if(watched == labelPause && event->type() == QEvent::MouseButtonPress) {
        if(isThreadExist && !isPausedNow) {
              return(pauseClicked());                                                 //  label pause event
        }
    }

    if(watched == labelStop && event->type() == QEvent::MouseButtonPress) {
          if(!isStoppedNow) {
                 return(stopClicked());                                               //  label stop event
          }
    }
    return false;
}

void MainWindow::addThread()
{
    WorkerCreator *creator = new WorkerCreator(&mMutex, &mWait);
    QThread* thread = new QThread;

    creator->moveToThread(thread);

    connect(thread, SIGNAL(started()), creator, SLOT(createWorker()),Qt::DirectConnection);
    connect(creator, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection );
    connect(this, SIGNAL(stopAll()), creator, SLOT(creatorStopSlot()),Qt::DirectConnection);
    connect(creator, SIGNAL(finished()), creator, SLOT(deleteLater()), Qt::DirectConnection);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()), Qt::DirectConnection);

    connect(creator, SIGNAL(signal1(int, int)), this, SLOT(printInfo(int, int)));

   connect(this, SIGNAL(mainSignalPause()), creator, SLOT(cr_slot_pause()), Qt::DirectConnection);
   connect(this, SIGNAL(mainSignalResume()), creator, SLOT(cr_slot_resume()), Qt::DirectConnection);

    thread->start();

    return ;
}

void MainWindow::printInfo(int x, int y)
{
    map->insert(x,y);
    points << QPointF( x, y );
    curve->setSamples(points);
    plot->replot();
}

void MainWindow::timerSlotForMainThread()         // отрисовка графика из гл. потока - происходит по таймеру
{
    int x = (qrand() % endline) + startline;
    startline = x+1;
    endline = endline+5;
    int y = (qrand() % 14);
    curve2->attach(plot);
    points2 << QPointF( x, y );
    curve2->setSamples(points2);
    plot->replot();
}

void MainWindow::pauseMainLine()
{
    timer1->stop();
}

void MainWindow::resumeMainLine()
{
    timer1->start(1500);
    isMainStopped = false;
}

void MainWindow::stopMainLine()
{
    if (!isMainStopped){
      timer1->stop();
      curve2->detach();
      points2.clear();
      plot->replot();
      startline = 2;
      endline = 6;
      isMainStopped = true;
    }
}


MainWindow::~MainWindow(){ delete ui; }

