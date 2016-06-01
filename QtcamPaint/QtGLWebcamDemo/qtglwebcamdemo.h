#ifndef QTGLWEBCAMDEMO_H
#define QTGLWEBCAMDEMO_H

#include <QMainWindow>

#include <opencv2/highgui/highgui.hpp>
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
namespace Ui {
    class QtGLWebcamDemo;
}

class QtGLWebcamDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtGLWebcamDemo(QWidget *parent = 0);
    ~QtGLWebcamDemo();

    /*
    QString sPath = QCoreApplication::applicationDirPath().append("/frontalface.xml");
    QFile::copy(":/android-sources/OpenCV/haarcascade_frontalface_default.xml" , sPath);
    cv::CascadeClassifier faceCascade = cv::CascadeClassifier(sPath.toStdString());
    QFile::remove(sPath);
    */

    const char *filename="C:\Users\Qitay\Desktop\QtOpenCVViewerGl-master\QtGLWebcamDemo\hand3.xml";
    cv::CascadeClassifier faceCascade = cv::CascadeClassifier(filename);

    //CascadeClassifier cascade = ( CvHaarClassifierCascade* )cvLoad(filename,0,0,0);

    //CascadeClassifier hand_cascade(filename);
    /*
    const char *filename="C://OpenCV2.2//data//haarcascades//haarcascade_frontalface_alt.xml";
    CascadeClassifier cascade = ( CvHaarClassifierCascade* )cvLoad(filename,0,0,0);

    const char *filename="C://OpenCV2.2//data//haarcascades//haarcascade_frontalface_alt.xml";
    CascadeClassifier cascade = ( CvHaarClassifierCascade* )cvLoad(filename,0,0,0);

    const char *filename="C://OpenCV2.2//data//haarcascades//haarcascade_frontalface_alt.xml";
    CascadeClassifier cascade = ( CvHaarClassifierCascade* )cvLoad(filename,0,0,0);
    */
    /*
    CascadeClassifier hand_cascade("hand3.xml");
    CascadeClassifier hand_cascade1("fist1.xml");
    CascadeClassifier hand_cascade2("fist2.xml");
    CascadeClassifier hand_cascade3("palm1.xml");
    CascadeClassifier hand_cascade4("palm2.xml");
    CascadeClassifier face_cascade("face1.xml");
*/
    //VideoCapture cap(0);	// Capture source - default is 0

    // working mode checks
    bool useHaar = true;
    bool useSkinDet = false;

    int cascadeIndex = 1; // current cascade index
    char waitkey = '-';   // last read key - when not clicked is '-'

    int backgroundFrame = 500; // number of frames to initialize background

                    // Materials:
    Mat frame;		// one frame of capture
    Mat back;		// background image
    Mat fore;		// foreground image
    Mat detection;	// skin color detection
    Mat image;		// our painting

                    // Skin color detetction:
    Mat ycbcr;		// YCbCr
    Mat hsv;		// Hue Saturation Value

    // Windows shown - not needed here:
    //namedWindow("Frame", WINDOW_NORMAL);
    //namedWindow("Drawing", WINDOW_AUTOSIZE);
    //namedWindow("Detection");
    //namedWindow("Background");
    //namedWindow("HSV");
    //namedWindow("YCBCR");

    // Position and frame size
    int posX = 0;
    int posY = 0;
    int lastX;
    int lastY;
    int frameH;
    int frameW;
    //Mat3b frame;


    int R = 0;
    int B = 0;
    int G = 0;
    int thickness = 3; // default brush size

    // Read background for our painting
    /*
    image = imread("white2.png", CV_LOAD_IMAGE_COLOR);
    if (!image.data)
    {
        cout << "Nie można otworzyć obrazu!" << std::endl;
       // return -1;
    }
*/
    vector<Point> palm_points;
    vector<pair<double, int> > distvec;
    vector<pair<Point, double> > palm_centers; // Center of tracked object



private slots:
    void on_actionStart_triggered();

    void on_actionVertical_Flip_triggered(bool checked);

    void on_action_Horizontal_Mirror_triggered(bool checked);

private:
    Ui::QtGLWebcamDemo *ui;

    cv::VideoCapture cap;

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool mFlipVert;
    bool mFlipHoriz;




};

#endif // QTGLWEBCAMDEMO_H
