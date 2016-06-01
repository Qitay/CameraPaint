#include <QApplication>
#include <QException>
#include <QDebug>
#include "qtglwebcamdemo.h"
#include "ui_qtglwebcamdemo.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;
using namespace cv;

#define blue  CV_RGB(0,0,255)
#define green CV_RGB(0,255,0)
#define red   CV_RGB(255,0,0)
#define white CV_RGB(255,255,255)
#define black CV_RGB(0,0,0)

#include "qtglwebcamdemo.h"

int main(int argc, char *argv[])
{
    int res=-1;

    try
    {
        QApplication a(argc, argv);
        QtGLWebcamDemo w;



        w.show();

        res = a.exec();
    }
    catch(QException &e)
    {
        qCritical() << QString("Exception: %1").arg( e.what() );
    }
    catch(...)
    {
        qCritical() << QString("Unhandled Exception");
    }

    return res;
}
