#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "TrackingObject.h"

using namespace std;
using namespace cv;

#pragma once
class Tracker
{
public:
	void Add(vector<TrackingObject> _newTrackingObjects);
	vector<TrackingObject*> GetAllTrackingObjects();
private:
	vector<TrackingObject> _currentTrackingObjects;
	double GetDistanceBetweenPoints(Point point1, Point point2);
	void AddToExistingTrackingObject(TrackingObject _newObject, TrackingObject* _existingObject);
	void AddNewTrackingObject(TrackingObject _newObject);
};

