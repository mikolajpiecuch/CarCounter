// CarCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "TrackingObject.h"
#include "Tracker.h"

#define ROI_HEIGHT 300
#define MINIMUM_AREA 3000
#define RADIUS 15
#define RED Scalar(0, 0, 255)
#define GREEN Scalar(0, 255, 0)
#define BLUE Scalar(255, 0, 0)


using namespace cv;
using namespace std;

Rect CreateRectForROI(int height, int width, int ROIHeight);
void DrawCounter(Mat frame, long number);
bool CarCrossedLine(TrackingObject car, int height);

Ptr<BackgroundSubtractor> pMOG2;

int main()
{
	Mat orgImage, blurImage, mask;
	VideoCapture cap = VideoCapture("..\\sample\\traffic_surveillance.mp4");
	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Mask", CV_WINDOW_AUTOSIZE);
	pMOG2 = createBackgroundSubtractorMOG2(10000, 1000, false);
	Rect rect = CreateRectForROI(cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FRAME_WIDTH), ROI_HEIGHT);
	long counter = 0;
	Tracker carTracker;

	while (1)
	{
		if (!cap.read(orgImage)) 
		{
			break;
		}
		
		//subtract background
		Mat roi = orgImage(rect);
		pMOG2->apply(roi, mask);
		line(roi, Point(0, roi.size().height / 2), Point(roi.size().width, roi.size().height / 2), BLUE, 3);


		//morphological transformation
		GaussianBlur(mask, mask, Size(5, 5), 0);
		GaussianBlur(mask, mask, Size(5, 5), 0);
		Mat element5x5 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
		//morphologyEx(mask, mask, MORPH_CLOSE, element);*/
		dilate(mask, mask, element5x5);
		dilate(mask, mask, element5x5);
		erode(mask, mask, element5x5);
		threshold(mask, mask, 80, 255, THRESH_TRIANGLE);


		//finding contours
		vector<vector<Point>> contours;
		findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<TrackingObject> cars;
		for (int i = 0; i < contours.size(); i++)
		{
			double area = contourArea(contours[i], false);
			if (area < MINIMUM_AREA)
			{
				continue;
			}

			cars.push_back(TrackingObject(contours[i]));
		}

		carTracker.Add(cars);
		vector<TrackingObject*> trackingCars = carTracker.GetAllTrackingObjects();

		for (int i = 0; i < trackingCars.size(); i++)
		{
			TrackingObject* car = trackingCars[i];

			//draw bounding rect, center of object and line from first track
			int x = car->center.x;
			int y = car->center.y;
			circle(roi, car->center, RADIUS, RED, 2);
			line(roi, Point(x - RADIUS, y), Point(x + RADIUS, y), RED, 2);
			line(roi, Point(x, y - RADIUS), Point(x, y + RADIUS), RED, 2);
			rectangle(roi, car->rect, GREEN, 2);
			Point point1;
			Point point2;
			trackingCars[i]->GetLineBasedOnLatestPositions(&point1, &point2);
			line(roi, point1, point2, RED, 2);

			//check if car crossed line
			if (CarCrossedLine(*car, roi.size().height / 2))
			{
				car->counted = true;
				counter++;
				line(roi, Point(0, roi.size().height / 2), Point(roi.size().width, roi.size().height / 2), GREEN, 3);
			}
			//polylines(roi, trackingCars[i].latestPositions, false, RED, 3);
		}

		DrawCounter(orgImage, counter);
		imshow("Original", orgImage);
		imshow("Mask", mask);
		char ch = waitKey(10);
		if (ch == 27)
		{
			break;
		}
	}

	cap.release();
	cvDestroyAllWindows();
    return 0;
}

Rect CreateRectForROI(int height, int width, int ROIHeight) 
{
	int offset = 30;
	return Rect(offset, height - offset - ROIHeight, width - offset, ROIHeight);
}

void DrawCounter(Mat frame, long number) 
{
	stringstream ss;
	ss << "Counter: " << number;
	putText(frame, ss.str(), Point(30, 30), FONT_HERSHEY_PLAIN, 2, RED, 3);
}

bool CarCrossedLine(TrackingObject car, int height)
{
	Point point1 = car.latestPositions.front();
	Point point2 = car.latestPositions.back();

	return car.counted ? false : point1.y < height && point2.y > height || point2.y < height && point1.y > height;
}

