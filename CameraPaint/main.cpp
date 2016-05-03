#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<algorithm>
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
	if (abs(det) < TOL) { cout << "Punkty za blisko!" << endl; return make_pair(Point(0, 0), 0); }
	double idet = 1 / det;
	double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
	double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
	double radius = sqrt(pow(p2.x - centerx, 2) + pow(p2.y - centery, 2));
	return make_pair(Point(centerx, centery), radius);
}

int main(int argc, char *argv[])
{

	Mat frame;
	Mat back;
	Mat fore;
	vector<pair<Point, double> > palm_centers;
	VideoCapture cap(3);
	BackgroundSubtractorMOG2 bg;
	bg.set("nmixtures", 3);
	bg.set("detectShadows", false);
	namedWindow("Frame");
	namedWindow("Background");
	//namedWindow("Drawing", WINDOW_AUTOSIZE);
	namedWindow("Drawing");
	int backgroundFrame = 500;

	Mat image;
	image = imread("white1.png", CV_LOAD_IMAGE_COLOR);   
	if (!image.data)                             
	{
		cout << "Nie mo�na otworzy� obrazu!" << std::endl;
		return -1;
	}	
	int posX = 0;
	int posY = 0;

	for (;;)
	{
		vector<vector<Point> > contours;
		cap >> frame;
		if (backgroundFrame>0)
		{
			bg.operator ()(frame, fore); backgroundFrame--;
		}
		else
		{
			bg.operator()(frame, fore, 0);
		}
		bg.getBackgroundImage(back);
		erode(fore, fore, Mat());
		dilate(fore, fore, Mat());
		findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		for (int i = 0; i<contours.size(); i++)
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
			if (hullsI[0].size()>0)
			{
				Point2f rect_points[4]; rect.points(rect_points);
				for (int j = 0; j < 4; j++)
					line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
				Point rough_palm_center;
				convexityDefects(tcontours[0], hullsI[0], defects);
				if (defects.size() >= 3)
				{
					vector<Point> palm_points;
					for (int j = 0; j<defects.size(); j++)
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
					vector<pair<double, int> > distvec;
					for (int i = 0; i<palm_points.size(); i++)
						distvec.push_back(make_pair(dist(rough_palm_center, palm_points[i]), i));
					sort(distvec.begin(), distvec.end());

					//Keep choosing 3 points till you find a circle with a valid radius
					//As there is a high chance that the closes points might be in a linear line or too close that it forms a very large circle
					pair<Point, double> soln_circle;
					for (int i = 0; i + 2<distvec.size(); i++)
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
					if (palm_centers.size()>10)
						palm_centers.erase(palm_centers.begin());

					Point palm_center;
					double radius = 0;
					for (int i = 0; i<palm_centers.size(); i++)
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
					int no_of_fingers = 0;
					for (int j = 0; j<defects.size(); j++)
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
					cout << "Palce: " << no_of_fingers << endl;

					int lastX = posX; //save x position as last
					int lastY = posY; //save y position as last
					posX = palm_center.x; //get new x position
					posY = palm_center.y; //get new y position

					if (lastX != 0 && lastY != 0)
					{
						if (std::abs(lastX - posX) < 50 && std::abs(lastY - posY) < 50) //aby zniwelowa� b��dne przeskoki
						{
							if (no_of_fingers == 0)
							{
								//putText(frame, "RYSOWANIE ", cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
							}
							if (no_of_fingers == 1)
							{
								//putText(frame, "JEDEN PALEC WI�C RYSUJEMY", cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
								line(image,Point(lastX,lastY),Point(posX,posY),Scalar(0,255,0),3,2);
							}
							else if (no_of_fingers > 1)
							{
								//putText(frame, "RYSUJEMY", cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
								line(image, Point(lastX, lastY), Point(posX, posY), Scalar(240, 17, 17), 3, 2);

							}
		
						}
						
					}
					
					//cv2.imwrite("pic.jpg", blank_image)
					//inputMat.copyTo(outputMat, maskMat);
					
				}
				
				
			}

		}
		if (backgroundFrame>0)
		imshow("Frame", frame);
		imshow("Background", back);
		imshow("Drawing", image);                 
		
		if (waitKey(10) >= 0) break;
	}
	return 0;
}