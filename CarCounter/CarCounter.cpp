// CarCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"opencv2/imgproc.hpp"
#include"opencv2/videoio.hpp"
#include<opencv2/highgui.hpp>
#include<opencv2/video.hpp>
#include<iostream>
#include<conio.h>
#include <Windows.h>

#define ROI_HEIGHT 300
#define MINIMUM_AREA 3000
#define RED Scalar(0, 0, 255)
#define GREEN Scalar(0, 255, 0)
#define BLUE Scalar(255, 0, 0)

using namespace cv;
using namespace std;

Rect CreateRectForROI(int height, int width, int ROIHeight);

void DrawCounter(Mat frame, long number);

Ptr<BackgroundSubtractor> pMOG2;

int main()
{
	Mat orgImage, blurImage, mask;
	VideoCapture cap = VideoCapture("..\\sample\\traffic_surveillance.mp4");
	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Mask", CV_WINDOW_AUTOSIZE);
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	pMOG2 = createBackgroundSubtractorMOG2(4000, 900, true);
	Rect rect = CreateRectForROI(cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FRAME_WIDTH), ROI_HEIGHT);
	long counter = 0;

	while (1)
	{
		if (!cap.read(orgImage)) 
		{
			break;
		}
		
		//bubtract background
		Mat roi = orgImage(rect);
		pMOG2->apply(roi, mask);
		line(roi, Point(0, roi.size().height / 2), Point(roi.size().width, roi.size().height / 2), BLUE, 3);

		//morphological transformation
		blur(mask, mask, Size(15, 15), Point(-1, -1));
		//Mat element = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
		//morphologyEx(mask, mask, MORPH_CLOSE, element);*/
		erode(mask.clone(), mask, Mat(), Point(-1, -1), 1);
		dilate(mask.clone(), mask, Mat(6, 6, CV_32F), Point(-1, -1), 1);
		threshold(mask, mask, 210, 255, THRESH_TRIANGLE);
		//finding contours
		vector<vector<Point>> contours;
		findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		vector<std::vector<cv::Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);
		}


		for (int i = 0; i < convexHulls.size(); i++)
		{
			double area = contourArea(convexHulls[i], false);
			if (area < MINIMUM_AREA)
			{
				continue;
			}
			Moments mom = moments(convexHulls[i], true);

			int x = mom.m10 / mom.m00;
			int y = mom.m01 / mom.m00;
			int radius = 15;
			
			//naive counting
			if (y < (roi.size().height / 2) + 5 && y > (roi.size().height / 2) - 5) 
			{
				counter++;
			}

			circle(roi, Point(x, y), radius, RED, 2);
			line(roi, Point(x - radius, y), Point(x + radius, y), RED, 2);
			line(roi, Point(x, y - radius), Point(x, y + radius), RED, 2);

			Rect rect = boundingRect(convexHulls[i]);
			rectangle(roi, rect, GREEN, 3);
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
	putText(frame, ss.str(), Point(30, 30), FONT_HERSHEY_PLAIN, 2, GREEN);
}