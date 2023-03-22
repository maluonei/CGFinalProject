#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector>
#include <QPoint>
#include <QWidget>
#include "primitive.h"

const float pi = 3.1415926535f;
const float Radicale3 = std::sqrt(3);

class HomogeneousCoord {
public:
	float v[3];

	HomogeneousCoord() { v[0] = 0.f; v[1] = 0.f; v[2] = 1.f; }
	HomogeneousCoord(float _x, float _y, float _z = 1.f) {
		v[0] = _x;
		v[1] = _y;
		v[2] = _z;
	}

	float x() {
		for (int i = 0; i < 3; i++) {
			v[i] /= v[2];
		}
		return v[0];
	}

	float y() {
		for (int i = 0; i < 3; i++) {
			v[i] /= v[2];
		}
		return v[1];
	}

	float& operator[](int i) {
		return v[i];
	}

	float operator[](int i) const {
		return v[i];
	}

};

class Mat3 {
public:
	float m[3][3];
	float* u = m[0];
	float* v = m[1];
	float* w = m[2];

	Mat3(float v = 0.0f) {
		std::fill(m[0], m[0] + 9, v);
	}

	float* operator[](int i) {
		return m[i];
	}

	const float* operator[](int i) const {
		return m[i];
	}

	static Mat3 GetTransformMatrix(float _x, float _y) {
		Mat3 transMart;
		transMart.u[0] = 1;
		transMart.v[1] = 1;
		transMart.w[2] = 1;
		transMart.u[2] = _x;
		transMart.v[2] = _y;
		return transMart;
	}

	static Mat3 GetRotateMatrix(float angle) {
		Mat3 rotateMart;
		float cosa = cos(angle);
		float sina = sin(angle);
		rotateMart.u[0] = cosa;
		rotateMart.u[1] = -sina;
		rotateMart.v[0] = sina;
		rotateMart.v[1] = cosa;
		rotateMart.w[2] = 1;
		return rotateMart;
	}

	static Mat3 GetScaleMatrix(float mulx, float muly) {
		Mat3 scaleMatr;
		scaleMatr.u[0] = mulx;
		scaleMatr.v[1] = muly;
		scaleMatr.w[2] = 1;
		return scaleMatr;
	}

	static Mat3 GetScaleMatrix(float mul) {
		return GetScaleMatrix(mul, mul);
	}

	static Mat3 GetShearMatrix(float angle, bool isXAxis) {
		Mat3 shearMatr;
		shearMatr[0][0] = 1;
		shearMatr[1][1] = 1;
		shearMatr[2][2] = 1;
		if (isXAxis) {
			shearMatr[0][1] = tan(angle);
		}
		else {
			shearMatr[1][0] = tan(angle);
		}
		return shearMatr;
	}
};

Mat3 operator*(const Mat3& m1, const Mat3& m2) {
	Mat3 res;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				res[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return res;
}

HomogeneousCoord operator*(const Mat3& m1, const HomogeneousCoord& c1) {
	HomogeneousCoord h(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			h[i] += m1[i][j] * c1[j];
		}
	}
	return h;
}

class Shapee {
public:
	Shapee() {}
	virtual void draw(QWidget* qwidget) = 0;
	virtual void transform(float x, float y) = 0;
	virtual void rotate(float angle, HomogeneousCoord center = HomogeneousCoord(0, 0)) = 0;
	virtual void scale(float x, float y, HomogeneousCoord center = HomogeneousCoord(0, 0)) = 0;
	virtual void stear(float angle, bool isXAxis) = 0;
	virtual HomogeneousCoord* center() = 0;
	virtual Shapee* copy() = 0;
};

class Rect : public Shapee {
public:
	HomogeneousCoord c0, c1, c2, c3;
	Rect() {}
	Rect(HomogeneousCoord _c1, HomogeneousCoord _c2) {
		this->c0 = HomogeneousCoord(std::min(_c1.x(), _c2.x()), std::min(_c1.y(), _c2.y()));
		this->c1 = HomogeneousCoord(std::max(_c1.x(), _c2.x()), std::min(_c1.y(), _c2.y()));
		this->c2 = HomogeneousCoord(std::max(_c1.x(), _c2.x()), std::max(_c1.y(), _c2.y()));
		this->c3 = HomogeneousCoord(std::min(_c1.x(), _c2.x()), std::max(_c1.y(), _c2.y()));
	}

	Rect(HomogeneousCoord _c0, HomogeneousCoord _c1, HomogeneousCoord _c2, HomogeneousCoord _c3) {
		this->c0 = _c0;
		this->c1 = _c1;
		this->c2 = _c2;
		this->c3 = _c3;
	}

	virtual void draw(QWidget* qwidget)override {
		BresenhamLine(qwidget, c0.x(), c0.y(), c1.x(), c1.y());
		BresenhamLine(qwidget, c1.x(), c1.y(), c2.x(), c2.y());
		BresenhamLine(qwidget, c2.x(), c2.y(), c3.x(), c3.y());
		BresenhamLine(qwidget, c3.x(), c3.y(), c0.x(), c0.y());
	}

	virtual void transform(float _x, float _y)override {
		Mat3 transformMatrix = Mat3::GetTransformMatrix(_x, _y);

		c0 = transformMatrix * c0;
		c1 = transformMatrix * c1;
		c2 = transformMatrix * c2;
		c3 = transformMatrix * c3;
	}

	virtual void rotate(float angle, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 rotateMatrix = Mat3::GetRotateMatrix((angle * pi) / 180.0);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * rotateMatrix * transformToCenter * c0;
		c1 = transformBack * rotateMatrix * transformToCenter * c1;
		c2 = transformBack * rotateMatrix * transformToCenter * c2;
		c3 = transformBack * rotateMatrix * transformToCenter * c3;
	}

	virtual void scale(float _x, float _y, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 scaleMatrix = Mat3::GetScaleMatrix(_x, _y);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * scaleMatrix * transformToCenter * c0;
		c1 = transformBack * scaleMatrix * transformToCenter * c1;
		c2 = transformBack * scaleMatrix * transformToCenter * c2;
		c3 = transformBack * scaleMatrix * transformToCenter * c3;
	}

	virtual HomogeneousCoord* center() override {
		return new HomogeneousCoord(((double)(c0.x() + c1.x() + c2.x() + c3.x())) / 4.0, ((double)(c0.y() + c1.y() + c2.y() + c3.y())) / 4.0);
	}

	virtual void stear(float angle, bool isXAxis) override {
		Mat3 shearMatrix = Mat3::GetShearMatrix((angle * pi) / 180, isXAxis);

		c0 = shearMatrix * c0;
		c1 = shearMatrix * c1;
		c2 = shearMatrix * c2;
		c3 = shearMatrix * c3;
	}

	virtual Rect* copy() override {
		Rect* newRect = new Rect(c0, c1, c2, c3);
		return newRect;
	}
};

class RegularRect :public Rect {
public:
	RegularRect() {}
	RegularRect(HomogeneousCoord _c1, HomogeneousCoord _c2) {
		float minx = min(_c1.x(), _c2.x());
		float maxx = max(_c1.x(), _c2.x());
		float miny = min(_c1.y(), _c2.y());
		float maxy = max(_c1.y(), _c2.y());

		if (maxx - minx < maxy - miny) {
			maxy = miny + maxx - minx;
		}
		else {
			maxx = minx + maxy - miny;
		}

		c0 = HomogeneousCoord(minx, miny);
		c1 = HomogeneousCoord(maxx, miny);
		c2 = HomogeneousCoord(maxx, maxy);
		c3 = HomogeneousCoord(minx, maxy);
	}

	RegularRect(HomogeneousCoord _c0, HomogeneousCoord _c1, HomogeneousCoord _c2, HomogeneousCoord _c3) {
		c0 = _c0;
		c1 = _c1;
		c2 = _c2;
		c3 = _c3;
	}

	virtual HomogeneousCoord* center() override {
		return new HomogeneousCoord(((double)(c0.x() + c1.x() + c2.x() + c3.x())) / 4.0, ((double)(c0.y() + c1.y() + c2.y() + c3.y())) / 4.0);
	}

	virtual RegularRect* copy() override {
		return new RegularRect(c0, c1, c2, c3);
	}
};

class Hexagon :public Shapee {
public:
	HomogeneousCoord c0, c1, c2, c3, c4, c5;
	Hexagon() {}
	Hexagon(float x0, float y0, float x1, float y1) {
		float length = 0;

		float minx = min(x0, x1);
		float maxx = max(x0, x1);
		float miny = min(y0, y1);
		float maxy = max(y0, y1);

		if (Radicale3 * (maxx - minx) < maxy - miny) {
			length = maxx - minx;
		}
		else {
			length = (maxy - miny) / Radicale3;
		}

		c0 = HomogeneousCoord(minx, miny);
		c1 = HomogeneousCoord(minx + length, miny);
		c2 = HomogeneousCoord(minx + length * 1.5f, miny + length * Radicale3 / 2.f);
		c3 = HomogeneousCoord(minx + length, miny + length * Radicale3);
		c4 = HomogeneousCoord(minx, miny + length * Radicale3);
		c5 = HomogeneousCoord(minx - length / 2.f, miny + length * Radicale3 / 2.f);
	}

	Hexagon(HomogeneousCoord _c0, HomogeneousCoord _c1) {
		new (this)Hexagon(_c0.x(), _c0.y(), _c1.x(), _c1.y());
	}

	Hexagon(HomogeneousCoord _c0, HomogeneousCoord _c1, HomogeneousCoord _c2, HomogeneousCoord _c3, HomogeneousCoord _c4, HomogeneousCoord _c5) {
		c0 = _c0;
		c1 = _c1;
		c2 = _c2;
		c3 = _c3;
		c4 = _c4;
		c5 = _c5;
	}

	virtual void draw(QWidget* qwidget)override {
		BresenhamLine(qwidget, c0.x(), c0.y(), c1.x(), c1.y());
		BresenhamLine(qwidget, c1.x(), c1.y(), c2.x(), c2.y());
		BresenhamLine(qwidget, c2.x(), c2.y(), c3.x(), c3.y());
		BresenhamLine(qwidget, c3.x(), c3.y(), c4.x(), c4.y());
		BresenhamLine(qwidget, c4.x(), c4.y(), c5.x(), c5.y());
		BresenhamLine(qwidget, c5.x(), c5.y(), c0.x(), c0.y());
	}

	virtual void transform(float _x, float _y)override {
		Mat3 transformMatrix = Mat3::GetTransformMatrix(_x, _y);

		c0 = transformMatrix * c0;
		c1 = transformMatrix * c1;
		c2 = transformMatrix * c2;
		c3 = transformMatrix * c3;
		c4 = transformMatrix * c4;
		c5 = transformMatrix * c5;
	}

	virtual void rotate(float angle, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 rotateMatrix = Mat3::GetRotateMatrix((angle * pi) / 180);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * rotateMatrix * transformToCenter * c0;
		c1 = transformBack * rotateMatrix * transformToCenter * c1;
		c2 = transformBack * rotateMatrix * transformToCenter * c2;
		c3 = transformBack * rotateMatrix * transformToCenter * c3;
		c4 = transformBack * rotateMatrix * transformToCenter * c4;
		c5 = transformBack * rotateMatrix * transformToCenter * c5;
	}

	virtual void scale(float _x, float _y, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 scaleMatrix = Mat3::GetScaleMatrix(_x, _y);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * scaleMatrix * transformToCenter * c0;
		c1 = transformBack * scaleMatrix * transformToCenter * c1;
		c2 = transformBack * scaleMatrix * transformToCenter * c2;
		c3 = transformBack * scaleMatrix * transformToCenter * c3;
		c4 = transformBack * scaleMatrix * transformToCenter * c4;
		c5 = transformBack * scaleMatrix * transformToCenter * c5;
	}

	virtual HomogeneousCoord* center() override {
		return new HomogeneousCoord((c0.x() + c1.x() + c2.x() + c3.x() + c4.x() + c5.x()) / 6.0, (c0.y() + c1.y() + c2.y() + c3.y() + c4.y() + c5.y()) / 6.0);
	}

	virtual void stear(float angle, bool isXAxis) {
		Mat3 shearMatrix = Mat3::GetShearMatrix((angle * pi) / 180, isXAxis);

		c0 = shearMatrix * c0;
		c1 = shearMatrix * c1;
		c2 = shearMatrix * c2;
		c3 = shearMatrix * c3;
		c4 = shearMatrix * c4;
		c5 = shearMatrix * c5;
	}

	virtual Hexagon* copy() override {
		return new Hexagon(c0, c1, c2, c3, c4, c5);
	}
};

const float cos18 = cos(0.1 * pi);
const float sin18 = sin(0.1 * pi);

class RegularStar :public Shapee {
public:
	HomogeneousCoord c0, c1, c2, c3, c4;

	RegularStar(QPoint p1, QPoint p2) {
		if (p1.x() > p2.x()) std::swap(p1, p2);
		float l = p2.x() - p1.x();
		float s = l / (2 * (1 + sin18));

		c0 = HomogeneousCoord(p1.x(), p1.y());
		c1 = HomogeneousCoord(p1.x() + l, p1.y());
		c2 = HomogeneousCoord(p1.x() + s * (1 - sin18 * (1 + 2 * sin18)), p1.y() + s * cos18 * (1 + 2 * sin18));
		c3 = HomogeneousCoord(p1.x() + s * (1 + sin18), p1.y() - s * cos18);
		c4 = HomogeneousCoord(p1.x() + s * (1 + 2 * sin18 + sin18 * (1 + 2 * sin18)), p1.y() + s * cos18 * (1 + 2 * sin18));
	}

	RegularStar(HomogeneousCoord _c0, HomogeneousCoord _c1) {
		if (_c0.x() > _c1.x()) std::swap(_c0, _c1);
		float l = _c1.x() - _c0.x();
		float s = l / (2 * (1 + sin18));

		c0 = HomogeneousCoord(_c0.x(), _c0.y());
		c1 = HomogeneousCoord(_c0.x() + l, _c0.y());
		c2 = HomogeneousCoord(_c0.x() + s * (1 - sin18 * (1 + 2 * sin18)), _c0.y() + s * cos18 * (1 + 2 * sin18));
		c3 = HomogeneousCoord(_c0.x() + s * (1 + sin18), _c0.y() - s * cos18);
		c4 = HomogeneousCoord(_c0.x() + s * (1 + 2 * sin18 + sin18 * (1 + 2 * sin18)), _c0.y() + s * cos18 * (1 + 2 * sin18));
	}

	RegularStar(HomogeneousCoord _c0, HomogeneousCoord _c1, HomogeneousCoord _c2, HomogeneousCoord _c3, HomogeneousCoord _c4) {
		c0 = _c0;
		c1 = _c1;
		c2 = _c2;
		c3 = _c3;
		c4 = _c4;
	}

	virtual void draw(QWidget* qwidget)override {
		BresenhamLine(qwidget, c0.x(), c0.y(), c1.x(), c1.y());
		BresenhamLine(qwidget, c1.x(), c1.y(), c2.x(), c2.y());
		BresenhamLine(qwidget, c2.x(), c2.y(), c3.x(), c3.y());
		BresenhamLine(qwidget, c3.x(), c3.y(), c4.x(), c4.y());
		BresenhamLine(qwidget, c4.x(), c4.y(), c0.x(), c0.y());
	}

	virtual void transform(float _x, float _y)override {
		Mat3 transformMatrix = Mat3::GetTransformMatrix(_x, _y);

		c0 = transformMatrix * c0;
		c1 = transformMatrix * c1;
		c2 = transformMatrix * c2;
		c3 = transformMatrix * c3;
		c4 = transformMatrix * c4;
	}

	virtual void rotate(float angle, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 rotateMatrix = Mat3::GetRotateMatrix((angle * pi) / 180);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * rotateMatrix * transformToCenter * c0;
		c1 = transformBack * rotateMatrix * transformToCenter * c1;
		c2 = transformBack * rotateMatrix * transformToCenter * c2;
		c3 = transformBack * rotateMatrix * transformToCenter * c3;
		c4 = transformBack * rotateMatrix * transformToCenter * c4;
	}

	virtual void scale(float _x, float _y, HomogeneousCoord center = HomogeneousCoord(0, 0))override {
		Mat3 transformToCenter = Mat3::GetTransformMatrix(-center.x(), -center.y());
		Mat3 scaleMatrix = Mat3::GetScaleMatrix(_x, _y);
		Mat3 transformBack = Mat3::GetTransformMatrix(center.x(), center.y());

		c0 = transformBack * scaleMatrix * transformToCenter * c0;
		c1 = transformBack * scaleMatrix * transformToCenter * c1;
		c2 = transformBack * scaleMatrix * transformToCenter * c2;
		c3 = transformBack * scaleMatrix * transformToCenter * c3;
		c4 = transformBack * scaleMatrix * transformToCenter * c4;
	}

	virtual HomogeneousCoord* center() override {
		float xCenter = 0.0f, yCenter = 0.0f;
		xCenter = (c0.x() + c1.x() + c2.x() + c3.x() + c4.x()) / 5.0;
		yCenter = (c0.y() + c1.y() + c2.y() + c3.y() + c4.y()) / 5.0;
		return new HomogeneousCoord(xCenter, yCenter);
	}

	virtual void stear(float angle, bool isXAxis) {
		Mat3 shearMatrix = Mat3::GetShearMatrix((angle * pi) / 180, isXAxis);

		c0 = shearMatrix * c0;
		c1 = shearMatrix * c1;
		c2 = shearMatrix * c2;
		c3 = shearMatrix * c3;
		c4 = shearMatrix * c4;
	}

	virtual RegularStar* copy() override {
		return new RegularStar(c0, c1, c2, c3, c4);
	}
};

class HomogeneousCoord4f {
public:
	float v[4];

	HomogeneousCoord4f() { v[0] = 0.f; v[1] = 0.f; v[2] = 0.f; v[3] = 1.f; }
	HomogeneousCoord4f(float _x, float _y, float _z, float _w = 1.f) {
		v[0] = _x;
		v[1] = _y;
		v[2] = _z;
		v[3] = _w;
	}

	float x() {
		for (int i = 0; i < 4; i++) {
			v[i] /= v[3];
		}
		return v[0];
	}

	float y() {
		for (int i = 0; i < 4; i++) {
			v[i] /= v[3];
		}
		return v[1];
	}

	float& operator[](int i) {
		return v[i];
	}

	float operator[](int i) const {
		return v[i];
	}
};



class cube {
	HomogeneousCoord4f h1;
};

#endif