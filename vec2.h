#pragma once
#ifndef VEC2_H
#define VEC2_H

#include<iostream>
using namespace std;

class vec2 {
public:
	int index;
	double x;
	double y;
	double angle;
	vec2() {}

	vec2(double _x, double _y, int _index = -1) :x(_x), y(_y), index(_index) {}

	vec2 operator-(const vec2& v) const {
		return vec2(x - v.x, y - v.y);
	}

	vec2 operator+(const vec2& v) const {
		return vec2(x + v.x, y + v.y);
	}

	double cross(const vec2& v)const {
		return x * v.y - v.x * y;
	}

	double dot(const vec2& v)const {
		return x * v.x + y * v.y;
	}

	bool operator==(const vec2& v) const {
		return x == v.x && y == v.y;
	}

	bool operator<(const vec2& v) const {
		if (x != v.x) return x < v.x;
		return y < v.y;
	}

	vec2 operator*(const double t) const {
		return vec2(t * x, t * y);
	}

	vec2 operator*=(const double t) {
		this->x *= t;
		this->y *= t;
		return *this;
	}

	vec2 normalize() const {
		return *this * (1.0f / sqrtf(x * x + y * y));
	}

	void setAngle(vec2 target, vec2 dir) {
		vec2 tp = (vec2(x, y) - target);
		tp = tp.normalize();
		angle = tp.dot(dir);
	}
};
typedef vec2 point2;

inline float getCosine(point2 v1, point2 v2) {
	return v1.normalize().dot(v2.normalize());
}

inline ostream& operator<<(ostream& out, vec2& v) {
	out << v.x << " " << v.y;
	return out;
}

inline bool comparator(const vec2& v1, const vec2& v2) {
	if (v1.angle != v2.angle) return v1.angle > v2.angle;
	else if (v1.x != v2.x) return v1.x > v2.x;
	else return v1.y > v2.y;
}


inline bool InTriangleTest(point2 p, point2 q, point2 r, point2 s) {
	vec2 pq = q - p;
	vec2 qr = r - q;
	vec2 rp = p - r;
	vec2 ps = s - p;
	vec2 qs = s - q;
	vec2 rs = s - r;

	double b1 = pq.cross(ps);
	double b2 = qr.cross(qs);
	double b3 = rp.cross(rs);

	return b1 >= 0 && b2 >= 0 && b3 >= 0;
}

inline bool InLeftTest(point2 p, point2 q, point2 r) {
	vec2 pq = q - p;
	vec2 pr = r - p;
	return pq.cross(pr) >= 0;
}

inline bool InLeftTest_noEqual(point2 p, point2 q, point2 r) {
	vec2 pq = q - p;
	vec2 pr = r - p;
	return pq.cross(pr) > 0;
}

inline bool ComparatorXmin(point2 p1, point2 p2) {
	if (p1.x != p2.x) {
		return p1.x < p2.x;
	}
	return p1.y < p2.y;
}

#endif