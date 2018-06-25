#include "stdafx.h"
#include "Tracker.h"
#include "TrackingObject.h"

#define DISTANCE_PER_FRAME 80.0

void Tracker::Add(vector<TrackingObject> _newTrackingObjects)
{
	for (int i = 0; i < _currentTrackingObjects.size(); i++)
	{
		_currentTrackingObjects[i].matchFound = false;
	}

	for (int i = 0; i < _newTrackingObjects.size(); i++)
	{
		int index;
		double distance = DBL_MAX;
		for (int j = 0; j < _currentTrackingObjects.size(); j++)
		{
			double tempDistance = GetDistanceBetweenPoints(_newTrackingObjects[i].center, _currentTrackingObjects[j].center);
			if (tempDistance < distance)
			{
				distance = tempDistance;
				index = j;
			}
		}

		if (distance < DISTANCE_PER_FRAME)
		{
			AddToExistingTrackingObject(_newTrackingObjects[i], &_currentTrackingObjects[index]);
		}
		else
		{
			AddNewTrackingObject(_newTrackingObjects[i]);
		}
	}

	for (int i = 0; i < _currentTrackingObjects.size(); i++)
	{
		if (!_currentTrackingObjects[i].matchFound)
		{
			_currentTrackingObjects[i].framesWithoutMatch++;
		}

		if (_currentTrackingObjects[i].framesWithoutMatch > 5)
		{
			_currentTrackingObjects.erase(_currentTrackingObjects.begin() + i);
		}
	}
}

vector<TrackingObject*> Tracker::GetAllTrackingObjects()
{
	vector<TrackingObject*> track;
	for (int i = 0; i < _currentTrackingObjects.size(); i++)
	{
		if (_currentTrackingObjects[i].matchFound)
		{
			track.push_back(&_currentTrackingObjects[i]);
		}
	}
	return track;
}

double Tracker::GetDistanceBetweenPoints(Point point1, Point point2)
{
	int x = abs(point1.x - point2.x);
	int y = abs(point1.y - point2.y);

	return sqrt(pow(x, 2) + pow(y, 2));
}

void Tracker::AddToExistingTrackingObject(TrackingObject _newObject, TrackingObject* _existingObject)
{
	_existingObject->latestPositions.push_back(_existingObject->center);
	_existingObject->center = _newObject.center;
	_existingObject->contour = _newObject.contour;
	_existingObject->rect = _newObject.rect;
	_existingObject->matchFound = true;
}

void Tracker::AddNewTrackingObject(TrackingObject _newObject)
{
	_currentTrackingObjects.push_back(_newObject);
}
