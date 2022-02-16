#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QLabel>
#include <QDebug>
#include "workercreator.h"
#include <QMap>
#include <QTimer>
#include <QPushButton>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_directpainter.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     bool eventFilter(QObject *watched, QEvent *event);

private:

    QMap<int,int> *map;
    QwtPlot *plot;
    QPolygonF points, points2;
    QwtPlotCurve *curve = nullptr;
    QwtPlotCurve *curve2 = nullptr;

    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout1;
    QHBoxLayout *hlayout2;

    QLabel *labelStart, *labelPause, *labelStop, *labelText1, *labelText2, *labelText3;
    QPushButton *btnStartMain, *btnPauseMain, *btnStopMain;
    bool isThreadExist, isPausedNow, isStoppedNow, isMainStopped;

    bool startClicked();
    bool pauseClicked();
    bool stopClicked();
    void addThread();

    int startline = 3, endline = 6;

    QMutex mMutex;
    QWaitCondition mWait;

    QTimer *timer1;

    Ui::MainWindow *ui;

signals:
    void stopAll();

    void mainSignalPause();
    void mainSignalResume();

private slots:
    void printInfo(int x, int y);
    void pauseMainLine();
    void resumeMainLine();
    void stopMainLine();

    void timerSlotForMainThread();


};

#endif // MAINWINDOW_H
