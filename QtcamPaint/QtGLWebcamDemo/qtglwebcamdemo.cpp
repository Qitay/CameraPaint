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

double dist(Point x, Point y)
{
    return (x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y);
}

pair<Point, double> circleFromPoints(Point p1, Point p2, Point p3)
{
    double offset = pow(p2.x, 2) + pow(p2.y, 2);
    double bc = (pow(p1.x, 2) + pow(p1.y, 2) - offset) / 2.0;
    double cd = (offset - pow(p3.x, 2) - pow(p3.y, 2)) / 2.0;
    double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);
    double TOL = 0.0000001;
    if (abs(det) < TOL) { /*cout << "Punkty za blisko!" << endl;*/ return make_pair(Point(0, 0), 0); }
    double idet = 1 / det;
    double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
    double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
    double radius = sqrt(pow(p2.x - centerx, 2) + pow(p2.y - centery, 2));
    return make_pair(Point(centerx, centery), radius);
}

QtGLWebcamDemo::QtGLWebcamDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtGLWebcamDemo)
{
    ui->setupUi(this);

    mFlipVert=false;
    mFlipHoriz=false;

}

QtGLWebcamDemo::~QtGLWebcamDemo()
{
    delete ui;
}

void QtGLWebcamDemo::on_actionStart_triggered()
{
    if( !cap.isOpened() )
        if( !cap.open( 0 ) )
            return;
    startTimer(0);
}

void QtGLWebcamDemo::timerEvent(QTimerEvent *event)
{
    //cv::Mat image;
   // cap >> image;

   // cap >> frame;
   // cap >> detection;

    /*
    if( mFlipVert && mFlipHoriz )
    {
        cv::flip( frame,frame, -1);
        cv::flip( detection,detection, -1);
    }
    else if( mFlipVert )
    {
        cv::flip( frame,frame, 0);
        cv::flip( detection,detection, 0);
    }
    else if( mFlipHoriz )
    {
        cv::flip( frame,frame, 1);
        cv::flip( detection,detection, 1);

    }*/



    cap >> frame;
    cap >> detection;



    frameH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    frameW = cap.get(CV_CAP_PROP_FRAME_WIDTH);

    //SKIN COLOR DETECTION
    if (useSkinDet)
    {
        flip(hsv, hsv, 1); //flips video
        flip(ycbcr, ycbcr, 1); //flips video

        //blur(src, src, Size(3, 3));
        cvtColor(detection, hsv, CV_BGR2HSV);
        cvtColor(detection, ycbcr, COLOR_BGR2YCrCb);


        inRange(hsv, Scalar(0, 48, 80), Scalar(20, 255, 255), hsv);
        inRange(ycbcr, Scalar(0, 133, 77), Scalar(255, 173, 127), ycbcr);
    }



    rectangle(frame, Point(0, 0), Point(100, 100), Scalar(255, 255, 255), 2, 8, 0);
    putText(frame, "KOLOR", Point(5, 50), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(255, 255, 255), 2, 8, 0);
    rectangle(frame, Point(0, 100), Point(100, 200), Scalar(0, 255, 0), 2, 8, 0);
    putText(frame, "KOLOR", Point(5, 150), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 255, 0), 2, 8, 0);
    rectangle(frame, Point(0, 200), Point(100, 300), Scalar(240, 17, 17), 2, 8, 0);
    putText(frame, "KOLOR", Point(5, 250), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(240, 17, 17), 2, 8, 0);
    rectangle(frame, Point(0, 300), Point(100, 400), Scalar(0, 0, 255), 2, 8, 0);
    putText(frame, "KOLOR", Point(5, 350), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);


    rectangle(frame, Point(540, 0), Point(640, 100), Scalar(0, 0, 0), 2, 8, 0);
    putText(frame, "USUN", Point(545, 60), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8, 0);
    rectangle(frame, Point(540, 100), Point(640, 200), Scalar(0, 0, 0), 2, 8, 0);
    line(frame, Point(550, 150), Point(630, 150), Scalar(0, 0, 0), 9, 2);
    rectangle(frame, Point(540, 200), Point(640, 300), Scalar(0, 0, 0), 2, 8, 0);
    line(frame, Point(550, 250), Point(630, 250), Scalar(0, 0, 0), 3, 2);
    rectangle(frame, Point(540, 300), Point(640, 400), Scalar(0, 0, 0), 2, 8, 0);
    putText(frame, "Zapisz", Point(545, 350), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);
    //putText(frame, "KOLOR", Point(5, 350), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);


    flip(frame, frame, 0); //flips video
    flip(detection, detection, 0); //flips video



    cv::Rect maxRect; // 0 sized rect
    std::vector<Rect> hands;

    // Cascade detection mode switch
    /*
    switch (cascadeIndex)
    {
    case 1:
        hand_cascade.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
        break;
    case 2:
        hand_cascade2.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
        break;
    case 3:
        hand_cascade3.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
        break;
    case 4:
        hand_cascade3.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
        break;
    case 5:
        face_cascade.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(400, 400));
        break;
    }
*/
    lastX = posX; //save x position as last
    lastY = posY; //save y position as last


        // Draw circles on the detected hands
        for (int i = 0; i < hands.size(); i++)
        {
            if (hands[i].area() > maxRect.area())
                maxRect = hands[i];
        }
        Point center(maxRect.x + maxRect.width*0.5, maxRect.y + maxRect.height*0.5);
        ellipse(frame, center, Size(maxRect.width*0.5, maxRect.height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
        circle(frame, center, 5, Scalar(144, 144, 255), 3);
        posX = maxRect.x + maxRect.width*0.5;
        posY = maxRect.y + maxRect.height*0.5;


    if (lastX != 0 && lastY != 0 && posX != 0 && posY != 0)
    {
        if (std::abs(lastX - posX) < 30 && std::abs(lastY - posY) < 30) //aby zniwelować błędne przeskoki
        {

                //line(image, Point(lastX, lastY), Point(posX, posY), Scalar(240, 17, 17), 3, 2);
                line(image, Point(lastX, lastY), Point(posX, posY), Scalar(R, B, G), thickness, 2);

            if(posX > 540 && posX < 640 && posY<100 && posY>0)
            {
                image = imread("white2.png", CV_LOAD_IMAGE_COLOR);
            }
            if (posX > 0 && posX < 100 && posY<100 && posY>0)
            {
                R = 255;
                B = 255;
                G = 255;

                putText(frame, "Wybrano biały", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX > 0 && posX < 100 && posY<200 && posY>100)
            {
                R = 0;
                B = 255;
                G = 0;

                putText(frame, "Wybrano zielony", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX > 0 && posX < 100 && posY<300 && posY>200)
            {
                R = 240;
                B = 17;
                G = 17;

                putText(frame, "Wybrano niebieski", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX > 0 && posX < 100 && posY<400 && posY>300)
            {
                R = 0;
                B = 0;
                G = 255;

                putText(frame, "Wybrano czerwony", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX > 540 && posX < 640 && posY<200 && posY>100)
            {
                thickness = 9;
                putText(frame, "Grubo", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX > 540 && posX < 640 && posY<300 && posY>200)
            {
                thickness = 3;
                putText(frame, "Mniej grubo", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
            if (posX>540 && posX < 640 && posY < 400 && posY>300)
            {
                string s = "screen-";
                time_t rawtime;
                struct tm * timeinfo;
                char buffer[80];
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                strftime(buffer, 80, "%d-%m-%Y-%I-%M-%S", timeinfo);
                std::string str(buffer);
                s += str;
                s += ".jpg";
                imwrite(s, image);
                cout << "Zapis pliku " << s << endl;
                putText(frame, "Zapisano", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
            }
        }
    }



    //putText(frame, "TERAZ RYSUJE", Point(150, 50), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(i, i, 255), 5, 8);


    // Display updated frames
   // imshow("Frame", frame);
  //  imshow("Drawing", image);
    //imshow("Detection", detection);
    //imshow("Background", back);

    // Display skin detection frames
    if (useSkinDet)
    {
        imshow("HSV", hsv);
        imshow("YCBCR", ycbcr);
    }

    // Buttons wait
    waitkey = waitKey(1);
    switch (waitkey)
    {
        case '-':{
            break;
        }
        case ' ':{
            cascadeIndex++;
            if (cascadeIndex > 5)
            {
                cascadeIndex = 1;
            }
            cout << "ZMIANA TRYBU! " << cascadeIndex << endl;
            break;
        }
        case 's':{
            string s = "picture-";

            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer, 80, "%d-%m-%Y-%I-%M-%S", timeinfo);
            std::string str(buffer);

            s += str;
            s += ".jpg";

            imwrite(s, image);
            cout << "Zapis pliku " << s << endl;
            break;
        }
        case 'h':{
            if (useHaar == false)
            {
                //putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
                cout << "ZMIANA TRYBU!" << endl;
                useHaar = true;
            }
            else
            {
                //putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
                cout << "ZMIANA TRYBU!" << endl;
                useHaar = false;
            }
            break;
        }
        case 'g':{
            if (useSkinDet == false)
            {
                //putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
                cout << "ZMIANA TRYBU!" << endl;
                useSkinDet = true;
            }
            else
            {
                //putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
                cout << "ZMIANA TRYBU!" << endl;
                useSkinDet = false;
            }
            break;
        }
        case 'q':{
            //return 0;
        }
    }
    waitkey = '-';

    // Do what you want with the image :-)

    // Show the image
    ui->openCVviewer->showImage( frame );
}

void QtGLWebcamDemo::on_actionVertical_Flip_triggered(bool checked)
{
    mFlipVert = checked;
}

void QtGLWebcamDemo::on_action_Horizontal_Mirror_triggered(bool checked)
{
    mFlipHoriz = checked;
}
