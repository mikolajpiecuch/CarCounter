#include "stdafx.h"
#include "TrackingObject.h"

#define MINIMUM_AREA 3000
#define RADIUS 15

TrackingObject::TrackingObject(vector<Point> _contour)
{
	Moments mom = moments(_contour, true);

	int x = mom.m10 / mom.m00;
	int y = mom.m01 / mom.m00;
	center = Point(x, y);
	rect = boundingRect(_contour);
	framesWithoutMatch = 0;
	latestPositions.push_back(center);
	counted = false;
}

void TrackingObject::GetLineBasedOnLatestPositions(Point * _point1, Point * _point2)
{
	*_point1 = latestPositions.front();
	*_point2 = latestPositions.back();
}
