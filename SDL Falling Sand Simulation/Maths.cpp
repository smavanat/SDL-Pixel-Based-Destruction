#include "Maths.h"
#include<stdio.h>

//Rotates a Vector3 about the z-axis and returns the result. Takes an angle in degrees or in radians, but if 
//the angle is in degrees, the inRadians parameter must be set to false;
Vector3 rotate(Vector3 vec, double angle, bool inRadians) {
	Vector3 ret = {};
	if (!inRadians) {
		angle *= DEGREES_TO_RADIANS;
	}
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = vec.x * sin(angle) + vec.y * cos(angle);
	ret.z = vec.z;
	return ret;
}

//Rotates a Vector2 about the z-axis and returns the result. Takes an angle in degrees or in radians, but if 
//the angle is in degrees, the inRadians parameter must be set to false;
Vector2 rotate(Vector2 vec, double angle, bool inRadians) {
	Vector2 ret = {};
	if (!inRadians) {
		angle *= DEGREES_TO_RADIANS;
	}
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = vec.x * sin(angle) + vec.y * cos(angle);
	return ret;
}

//Rotates a Vector2 about a point in the z-axis and returns the result. Takes an angle in degrees or in radians, 
//but if the angle is in degrees, the inRadians parameter must be set to false.
Vector2 rotateAboutPoint(Vector2 point, Vector2 centre, double angle, bool inRadians) {
	Vector2 ret = {};
	if (!inRadians) {
		angle *= DEGREES_TO_RADIANS;
	}
	ret.x = centre.x + (point.x - centre.x) * cos(angle) - (point.y - centre.y) * sin(angle);
	ret.y = centre.y + (point.x - centre.x) * sin(angle) + (point.y - centre.y) * cos(angle);
	return ret;
}

//Creates a new Vector2
Vector2 newVector2(int x, int y) {
	Vector2 ret = {};
	ret.x = x;
	ret.y = y;
	return ret;
}

//Creates a new Vector3
Vector3 newVector3(int x, int y, int z) {
	Vector3 ret = {};
	ret.x = x;
	ret.y = y;
	ret.z = z;
	return ret;
}