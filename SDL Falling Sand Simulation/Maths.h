#pragma once
#include<SDL.h>

const double DEGREES_TO_RADIANS = (M_PI / 180);

struct Vector2
{
	int x;
	int y;

	Vector2 operator +(const Vector2& a) const {
		Vector2 ret = {};
		ret.x = a.x + x;
		ret.y = a.x + x;
		return ret;
	}

	Vector2 operator -(const Vector2& a) const {
		Vector2 ret = {};
		ret.x = x - a.x;
		ret.y = y - a.x;
		return ret;
	}

	bool operator ==(const Vector2& a) const {
		if (a.x == x && a.y == y) return true;
		else return false;
	}
};

struct Vector3 {
	int x;
	int y;
	int z;

	Vector3 operator +(const Vector3& a) const {
		Vector3 ret = {};
		ret.x = a.x + x;
		ret.y = a.x + y;
		ret.z = a.z + z;
		return ret;
	}

	Vector3 operator -(const Vector3& a) const {
		Vector3 ret = {};
		ret.x = x - a.x;
		ret.y = y - a.y;
		ret.z = z - a.z;
		return ret;
	}

	bool operator ==(const Vector3& a) const {
		if (a.x == x && a.y == y && a.z == z) return true;
		else return false;
	}
};

//Rotates a Vector3 about the z-axis and returns the result. Takes an angle in degrees or in radians, but if 
//the angle is in degrees, the inRadians parameter must be set to false;
Vector3 rotate(Vector3 vec, double angle, bool inRadians);

//Rotates a Vector2 about the z-axis and returns the result. Takes an angle in degrees or in radians, but if 
//the angle is in degrees, the inRadians parameter must be set to false;
Vector2 rotate(Vector2 vec, double angle, bool inRadians);

//Rotates a Vector2 about a point in the z-axis and returns the result. Takes an angle in degrees or in radians, 
//but if the angle is in degrees, the inRadians parameter must be set to false.
Vector2 rotateAboutPoint(Vector2 point, Vector2 centre, double angle, bool inRadians);

//Creates a new Vector2
Vector2 newVector2(int x, int y);

//Creates a new Vector3
Vector3 newVector3(int x, int y, int z);