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

Vector3 rotate(Vector3 vec, double angle, bool inRadians);

Vector2 rotate(Vector2 vec, double angle, bool inRadians);

Vector2 rotateAboutPoint(Vector2 point, Vector2 centre, double angle, bool inRadians);

Vector2 newVector2(int x, int y);

Vector3 newVector3(int x, int y, int z);