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

//This function returns the square of the euclidean distance between 2 points.
double dist(Point x, Point y)
{
	return (x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y);
}

//This function returns the radius and the center of the circle given 3 points
//If a circle cannot be formed , it returns a zero radius circle centered at (0,0)
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

int main(int argc, char *argv[])
{
	// Haar cascade classifiers initialization
	
	CascadeClassifier eye_cascade("haarcascade_mcs_eyepair_small.xml");
	CascadeClassifier nose_cascade("haarcascade_mcs_nose.xml");

	CascadeClassifier hand_cascade("hand3.xml");
	CascadeClassifier hand_cascade1("fist1.xml");
	CascadeClassifier hand_cascade2("fist2.xml");
	CascadeClassifier hand_cascade3("palm1.xml");
	CascadeClassifier hand_cascade4("palm2.xml");
	CascadeClassifier face_cascade("face1.xml");

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

		// BACKGROUND FRAME OPERATOR
		
		if (useHaar == false)
		{
			if (backgroundFrame>0)
			{
				bg.operator()(frame, fore); backgroundFrame--;
			}
			else
			{
				bg.operator()(frame, fore, 0);
			}
			bg.getBackgroundImage(back); // set background

			erode(fore, fore, Mat());
			dilate(fore, fore, Mat());

			findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (int i = 0; i < contours.size(); i++)
			{
				//Ignore all small insignificant areas
				if (contourArea(contours[i]) >= 5000)
				{
					//Draw contour
					vector<vector<Point>> tcontours;
					tcontours.push_back(contours[i]);
					drawContours(frame, tcontours, -1, cv::Scalar(0, 0, 255), 2);

					//Detect Hull in current contour
					vector<vector<Point> > hulls(1);
					vector<vector<int> > hullsI(1);
					convexHull(Mat(tcontours[0]), hulls[0], false);
					convexHull(Mat(tcontours[0]), hullsI[0], false);
					drawContours(frame, hulls, -1, cv::Scalar(0, 255, 0), 2);

					//Find minimum area rectangle to enclose hand
					RotatedRect rect = minAreaRect(Mat(tcontours[0]));

					//Find Convex Defects
					vector<Vec4i> defects;
					if (hullsI[0].size() > 0)
					{
						Point2f rect_points[4]; rect.points(rect_points);
						for (int j = 0; j < 4; j++)
						{
							if (useHaar == false)
							{
								line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
							}
						}
						Point rough_palm_center;
						convexityDefects(tcontours[0], hullsI[0], defects);
						if (defects.size() >= 3)
						{
							for (int j = 0; j < defects.size(); j++)
							{
								int startidx = defects[j][0]; Point ptStart(tcontours[0][startidx]);
								int endidx = defects[j][1]; Point ptEnd(tcontours[0][endidx]);
								int faridx = defects[j][2]; Point ptFar(tcontours[0][faridx]);
								//Sum up all the hull and defect points to compute average
								rough_palm_center += ptFar + ptStart + ptEnd;
								palm_points.push_back(ptFar);
								palm_points.push_back(ptStart);
								palm_points.push_back(ptEnd);
							}

							//Get palm center by first getting the average of all defect points, this is the rough palm center,
							//Then you chose the closest 3 points ang get the circle radius and center formed from them which is the palm center.
							rough_palm_center.x /= defects.size() * 3;
							rough_palm_center.y /= defects.size() * 3;
							Point closest_pt = palm_points[0];

							for (int i = 0; i < palm_points.size(); i++)
							{
								distvec.push_back(make_pair(dist(rough_palm_center, palm_points[i]), i));
							}
							sort(distvec.begin(), distvec.end());

							//Keep choosing 3 points till you find a circle with a valid radius
							//As there is a high chance that the closes points might be in a linear line or too close that it forms a very large circle
							pair<Point, double> soln_circle;
							for (int i = 0; i + 2 < distvec.size(); i++)
							{
								Point p1 = palm_points[distvec[i + 0].second];
								Point p2 = palm_points[distvec[i + 1].second];
								Point p3 = palm_points[distvec[i + 2].second];
								soln_circle = circleFromPoints(p1, p2, p3);//Final palm center,radius
								if (soln_circle.second != 0)
									break;
							}

							//Find avg palm centers for the last few frames to stabilize its centers, also find the avg radius
							palm_centers.push_back(soln_circle);
							if (palm_centers.size() > 10)
								palm_centers.erase(palm_centers.begin());

							Point palm_center;
							double radius = 0;
							for (int i = 0; i < palm_centers.size(); i++)
							{
								palm_center += palm_centers[i].first;
								radius += palm_centers[i].second;
							}
							palm_center.x /= palm_centers.size();
							palm_center.y /= palm_centers.size();
							radius /= palm_centers.size();

							//Draw the palm center and the palm circle

							circle(frame, palm_center, 5, Scalar(144, 144, 255), 3);
							circle(frame, palm_center, radius, Scalar(144, 144, 255), 2);

							//Detect fingers by finding points that form an almost isosceles triangle with certain thesholds

							for (int j = 0; j < defects.size(); j++)
							{
								int startidx = defects[j][0]; Point ptStart(tcontours[0][startidx]);
								int endidx = defects[j][1]; Point ptEnd(tcontours[0][endidx]);
								int faridx = defects[j][2]; Point ptFar(tcontours[0][faridx]);
								//X o--------------------------o Y
								double Xdist = sqrt(dist(palm_center, ptFar));
								double Ydist = sqrt(dist(palm_center, ptStart));
								double length = sqrt(dist(ptFar, ptStart));

								double retLength = sqrt(dist(ptEnd, ptFar));
								//Change thresholds to improve performance
								if (length <= 3 * radius&&Ydist >= 0.4*radius&&length >= 10 && retLength >= 10 && max(length, retLength) / min(length, retLength) >= 0.8)
									if (min(Xdist, Ydist) / max(Xdist, Ydist) <= 0.8)
									{
										if ((Xdist >= 0.1*radius&&Xdist <= 1.3*radius&&Xdist<Ydist) || (Ydist >= 0.1*radius&&Ydist <= 1.3*radius&&Xdist>Ydist))
											line(frame, ptEnd, ptFar, Scalar(0, 255, 0), 1), no_of_fingers++;
									}
							}
							no_of_fingers = min(5, no_of_fingers);

							if (0 < palm_center.x < frameW && 0 < palm_center.y < frameH)
							{
								posX = palm_center.x; //get new x position
								posY = palm_center.y; //get new y position
							}
						}
					}
				}
			}
		}

					
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
					putText(frame, "Zapisano", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
					image = imread("white2.png", CV_LOAD_IMAGE_COLOR);
				}
				if (posX > 0 && posX < 100 && posY<100 && posY>0)
				{
					R = 255;
					B = 255;
					G = 255;

					putText(frame, "Wybrano bia³y", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
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
					putText(frame, "Zapisano", Point(150, 100), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 2, 8, 0);
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
				return 0;
			}
		}
		waitkey = '-';
		
		//if (waitKey(10) >= 0) break;
	}
	return 0;
}