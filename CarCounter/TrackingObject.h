#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

#pragma once
class TrackingObject
{
public:
	vector<Point> contour;
	Rect rect;
	Point center;
	vector<Point> latestPositions;
	int framesWithoutMatch;
	bool matchFound;
	bool counted;

	TrackingObject(vector<Point> _contour);
	void GetLineBasedOnLatestPositions(Point* _point1, Point* _point2);
};

