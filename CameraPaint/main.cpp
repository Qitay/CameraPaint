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

void CreateTextButton(Mat& frame,Point p1, Point p2, Scalar s, std::string text, Point text_p)
{

	rectangle(frame, p1, p2, s, 2, 8, 0);
	putText(frame, text, text_p, CV_FONT_HERSHEY_COMPLEX, 0.8, s, 2, 8, 0);

}
void CreateLineButton(Mat& frame, Point r_p1, Point r_p2, Point l_p1, Point l_p2, int lineType)
{

	rectangle(frame, r_p1, r_p2, Scalar(0, 0, 0), 2, 8, 0);
	line(frame, l_p1, l_p2, Scalar(0, 0, 0), lineType, 2);

}

void DrawCircles(Mat& frame, std::vector<Rect>& hands, cv::Rect& maxRect, int& posX, int& posY)
{
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

}

void SaveFile(Mat& frame)
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
	imwrite(s, frame);
	cout << "Zapis pliku " << s << endl;
}

void DisplayMessage(Mat& frame, Point p1, std::string text)
{
	putText(frame, text, p1, CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
}

int main(int argc, char *argv[])
{
	// Haar cascade classifiers initialization
	CascadeClassifier hand_cascade("hand3.xml");
	CascadeClassifier hand_cascade1("fist1.xml");
	CascadeClassifier hand_cascade2("fist2.xml");
	CascadeClassifier hand_cascade3("palm1.xml");
	CascadeClassifier hand_cascade4("palm2.xml");
	CascadeClassifier face_cascade("face1.xml");
	CascadeClassifier eye_cascade("haarcascade_mcs_eyepair_small.xml");
	CascadeClassifier nose_cascade("haarcascade_mcs_nose.xml");

	VideoCapture cap(0);	// Capture source - default is 0

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

	

	// Background subtractor
	BackgroundSubtractorMOG2 bg;	
	bg.set("nmixtures", 3);
	bg.set("detectShadows", false);

	// Windows shown - not needed here:
	namedWindow("Frame", WINDOW_NORMAL);
	namedWindow("Drawing", WINDOW_AUTOSIZE);
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
	image = imread("white2.png", CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		cout << "Nie mo¿na otworzyæ obrazu!" << std::endl;
		return -1;
	}

	// not haar
	int no_of_fingers = 0;
	vector<vector<Point>> contours;
	
	vector<Point> palm_points;
	vector<pair<double, int> > distvec;
	vector<pair<Point, double> > palm_centers; // Center of tracked object
	

	//-------------------------------------------------------------------------------------//
	while(1)
	{
		cap >> frame;
		cap >> detection;

		flip(frame, frame, 1); //flips video	
		flip(detection, detection, 1); //flips video

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

		//flip(hsv, hsv, 1); //flips video	
		//flip(ycbcr, ycbcr, 1); //flips video	

		//blur(src, src, Size(3, 3));
		//cvtColor(detection, hsv, CV_BGR2HSV);
		//cvtColor(detection, ycbcr, COLOR_BGR2YCrCb);

		//inRange(hsv, Scalar(0, 48, 80), Scalar(20, 255, 255), hsv);
		//inRange(ycbcr, Scalar(0, 133, 77), Scalar(255, 173, 127), ycbcr);

		CreateTextButton(frame, Point(0, 0), Point(100, 100), Scalar(255, 255, 255), "KOLOR", Point(5, 50));
		CreateTextButton(frame, Point(0, 100), Point(100, 200), Scalar(0, 255, 0), "KOLOR", Point(5, 150));
		CreateTextButton(frame, Point(0, 200), Point(100, 300), Scalar(240, 17, 17), "KOLOR", Point(5, 250));
		CreateTextButton(frame, Point(0, 300), Point(100, 400), Scalar(0, 0, 255), "KOLOR", Point(5, 350));
		CreateTextButton(frame, Point(540, 0), Point(640, 100), Scalar(0, 0, 0), "USUN", Point(545, 60));
		CreateTextButton(frame, Point(540, 300), Point(640, 400), Scalar(0, 0, 0), "Zapisz", Point(545, 350));
		CreateLineButton(frame, Point(540, 100), Point(640, 200), Point(550, 150), Point(630, 150),9);
		CreateLineButton(frame, Point(540, 200), Point(640, 300), Point(550, 250), Point(630, 250),3);


		// BACKGROUND FRAME OPERATOR
			
		//cout << "Palce: " << no_of_fingers << endl;
		//cout << frameH << " " << frameW << endl;
					
		cv::Rect maxRect; // 0 sized rect
		std::vector<Rect> hands;

		// Cascade detection mode switch
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
		case 6:
			eye_cascade.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
			break;
		case 7:
			nose_cascade.detectMultiScale(detection, hands, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(50, 50), Size(300, 300));
			break;
		}

		lastX = posX; //save x position as last
		lastY = posY; //save y position as last

		if (useHaar == true)
		{
			// Draw circles on the detected hands

			DrawCircles(frame, hands,maxRect,  posX,  posY);
		}

		if (lastX != 0 && lastY != 0 && posX != 0 && posY != 0)
		{
			if (std::abs(lastX - posX) < 30 && std::abs(lastY - posY) < 30) //aby zniwelowaæ b³êdne przeskoki
			{
				if (useHaar == false)
				{
					//line(image, Point(lastX, lastY), Point(posX, posY), Scalar(0, 255, 0), 3, 2);
					line(image, Point(lastX, lastY), Point(posX, posY), Scalar(R, B, G), thickness, 2);
				}
				else
				{
					//line(image, Point(lastX, lastY), Point(posX, posY), Scalar(240, 17, 17), 3, 2);
					line(image, Point(lastX, lastY), Point(posX, posY), Scalar(R, B, G), thickness, 2);
				}
				if(posX > 540 && posX < 640 && posY<100 && posY>0)
				{
					SaveFile(image);
					image = imread("white2.png", CV_LOAD_IMAGE_COLOR);
					DisplayMessage(frame, Point(150, 120), "Wyczyszczono");
				}
				if (posX > 0 && posX < 100 && posY<100 && posY>0)
				{
					R = 255;
					B = 255;
					G = 255;

					//putText(frame, "Wybrano bia³y", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Wybrano bialy");
				}
				if (posX > 0 && posX < 100 && posY<200 && posY>100)
				{
					R = 0;
					B = 255;
					G = 0;

					//putText(frame, "Wybrano zielony", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Wybrano zielony");

				}
				if (posX > 0 && posX < 100 && posY<300 && posY>200)
				{
					R = 240;
					B = 17;
					G = 17;

					//putText(frame, "Wybrano niebieski", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Wybrano niebieski");
				}
				if (posX > 0 && posX < 100 && posY<400 && posY>300)
				{
					R = 0;
					B = 0;
					G = 255;

					//putText(frame, "Wybrano czerwony", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Wybrano czerwony");
				}
				if (posX > 540 && posX < 640 && posY<200 && posY>100)
				{
					thickness = 9;
					//putText(frame, "Grubo", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Grubo");
				}
				if (posX > 540 && posX < 640 && posY<300 && posY>200)
				{
					thickness = 3;
					//putText(frame, "Mniej grubo", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					DisplayMessage(frame, Point(150, 100), "Mniej grubo");
				}
				if (posX>540 && posX < 640 && posY < 400 && posY>300)
				{
					SaveFile(image);

					DisplayMessage(frame, Point(150, 100), "Zapisano");
				}
			}
		}


		//putText(frame, "TERAZ RYSUJE", Point(150, 50), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(i, i, 255), 5, 8);
			

		// Display updated frames
		imshow("Frame", frame);
		imshow("Drawing", image);
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
				if (cascadeIndex > 7)
				{
					cascadeIndex = 1;
				}
				cout << "ZMIANA TRYBU! " << cascadeIndex << endl;
				break;
			}
			case 's':{
				SaveFile(image);
				break;
			}
			case 'h':{
				if (useHaar == false)
				{
					//putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
					cout << "ZMIANA TRYBU ON!" << endl;
					useHaar = true;
				}
				else
				{
					//putText(frame, "OpenCV forever!", Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 3, Scalar(i, i, 255), 5, 8);
					cout << "ZMIANA TRYBU OFF!" << endl;
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
				return 0;
			}
		}
		waitkey = '-';
		
		//if (waitKey(10) >= 0) break;
	}
	return 0;
}