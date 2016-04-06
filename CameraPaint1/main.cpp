#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	VideoCapture cap(0); //przechwycenie obrazu z kamery

	if (!cap.isOpened())
	{
		cout << "B³¹d! Nie mo¿na otworzyæ kamerki" << endl;
		return -1;
	}

	namedWindow("Kontrola", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Tworzenie suwaków w oknie Kontrola
	cvCreateTrackbar("Niskie H", "Kontrola", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("Wysokie H", "Kontrola", &iHighH, 179);

	cvCreateTrackbar("Niskie S", "Kontrola", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("Wysokie S", "Kontrola", &iHighS, 255);

	cvCreateTrackbar("Niskie V", "Kontrola", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("Wysokie V", "Kontrola", &iHighV, 255);

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // wczytanie klatki z wideo

		if (!bSuccess) 
		{
			cout << "B³¹d! Nie mo¿na odczytaæ klatki z wideo" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Zamiana z RGB na HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Thresholdowanie
		//usuñ ma³e obiekty z pierwszego planu
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//wype³nienie luk na pierwszym planie
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		imshow("Threshold", imgThresholded); //pokazuj tresholdowany obraz
		imshow("Oryginalny obraz", imgOriginal); //poka¿ oryginalny obraz

		if (waitKey(30) == 27)
		{
			cout << "ESC" << endl;
			break;
		}
	}

	return 0;

}