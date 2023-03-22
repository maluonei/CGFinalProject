#pragma once
#ifndef CURVE_H
#define CURVE_H
#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <QTextEdit>
#include <vector>
#include <sstream>
#include "primitive.h"
#include <Eigen/Dense>

#define maxCarNum 20;

using namespace Eigen;

float LagrangeCurveb(std::vector<QPoint>& points, int i, float x) {
	int length = points.size();
	float ans = 1;
	float ir = 1.f * i / (length - 1);
	for (int j = 0; j < length; j++) {
		if (i == j) continue;
		float jr = 1.f * j / (length - 1);
		ans *= (x - jr) / (ir - jr);
	}
	return ans;
}

QPointF LagrangeCurvef(std::vector<QPoint>& points, float x) {
	int length = points.size();
	QPointF ans(0, 0);
	for (int i = 0; i < length; i++) {
		float para = LagrangeCurveb(points, i, x);
		ans.setX(ans.x() + points[i].x() * para);
		ans.setY(ans.y() + points[i].y() * para);
	}
	return ans;
}

void LagrangeCurve(QWidget* widget, std::vector<QPoint>& points) {
	int size = points.size();
	int pointNum = size * 1000;
	QPointF p;
	for (int i = 0; i < size; i++) {
		DrawBigPoint(widget, points[i].x(), points[i].y());
	}
	if (size <= 1) return;
	QPainter painter(widget);
	for (int i = 0; i < pointNum; i++) {
		p = LagrangeCurvef(points, (i * 1.0f) / (pointNum - 1));
		painter.drawPoint(p.x(), p.y());
	}
}

QPointF CalCardianlCurve(QPointF& a0, QPointF& a1, QPointF& a2, QPointF& a3, float x) {
	return a0 + a1 * x + a2 * x * x + a3 * x * x * x;
}

void CardCurve(QWidget* widget, vector<QPoint>& points, float t) {
	float s = (1.0 - t) / 2;
	//QPoint f0 = points[1];
	//QPoint f1 = points[2];
	//QPoint f0p = s * (points[2] - points[0]);
	//QPoint f1p = s * (points[3] - points[1]);

	/*
	B = C-1 =	    [0,   1,    0,  0,
				-s,   0,    s,  0,
				2s, s-3, 3-2s,  -s,
				-s, 2-s,  s-2,   s]


	*/

	QPointF a0 = points[1];
	QPointF a1 = -s * points[0] + s * points[2];
	QPointF a2 = 2 * s * points[0] + (s - 3) * points[1] + (3 - 2 * s) * points[2] - s * points[3];
	QPointF a3 = -s * points[0] + (2 - s) * points[1] + (s - 2) * points[2] + s * points[3];


	QPainter painter(widget);
	int pointNum = 2000;
	for (int i = 0; i < pointNum; i++) {
		painter.drawPoint(CalCardianlCurve(a0, a1, a2, a3, (i * 1.0f) / (pointNum - 1)));
	}

	//Matrix4f
}

void CardCurveAll(QWidget* widget, vector<QPoint>& points, float t) {
	float s = (1.0 - t) / 2;
	int length = points.size() > 20 ? 20 : points.size();
	Matrix<float, 20, 20> C = Matrix<float, 20, 20>::Zero(), B = Matrix<float, 20, 20>::Zero();
	for (int i = 1; i < length - 1; i++) {
		float u = ((i - 1) * 1.0f / (length - 3));
		for (int j = 0; j < length; j++) {
			C(i, j) = pow(u, j);
		}
	}
	for (int i = 0; i < length; i++) {
		C(0, i) = C(2, i);
		C(length - 1, i) = C(length - 3, i);
	}
	C(0, 1) -= (1 / s);
	for (int i = 1; i < length; i++) {
		C(length - 1, i) += (i / s);
	}
	for (int i = length; i < 20; i++) {
		C(i, i) = 1;
	}
	B = C.inverse();

	Matrix<float, 20, 1> px = Matrix<float, 20, 1>::Zero(), py = Matrix<float, 20, 1>::Zero(), ax = Matrix<float, 20, 1>::Zero(), ay = Matrix<float, 20, 1>::Zero();
	for (int i = 0; i < length; i++) {
		px(i, 0) = points[i].x();
	}
	for (int i = 0; i < length; i++) {
		py(i, 0) = points[i].y();
	}

	ax = B * px;
	ay = B * py;

	QPainter painter(widget);
	int pointNum = 500 * length;
	for (int i = 0; i < pointNum; i++) {
		QPointF ans(0, 0);
		for (int j = 0; j < length; j++) {
			ans.setX(ans.x() + ax(j, 0) * pow((i * 1.0f) / (pointNum - 1), j));
			ans.setY(ans.y() + ay(j, 0) * pow((i * 1.0f) / (pointNum - 1), j));
		}
		painter.drawPoint(ans);
	}
	return;
}

int C(int n, int k) {
	if (k == 0) return 1;
	int ans = 1;
	for (int i = n; i > n - k; i--) {
		ans *= i;
	}
	for (int i = 2; i <= k; i++) {
		ans /= i;
	}
	return ans;
}

void Bazier(QWidget* widget, vector<QPoint>& points, float t) {
	int length = points.size();
	int pointNum = 500 * length;
	QPainter painter(widget);
	for (int i = 0; i < pointNum; i++) {
		QPointF p(0, 0);
		float u = i / (pointNum - 1.0f);
		for (int j = 0; j < length; j++) {
			p += points[j] * C(length - 1, j) * pow(u, j) * pow(1 - u, length - 1 - j);
		}
		painter.drawPoint(p);
	}
}

void decase(QWidget* widget, vector<QPointF>& points, int n, int Pointnum) {
	float delta = 1.0 / (double)Pointnum;
	float t = 0.0;
	int i, m;
	vector<QPointF> R(n + 1), Q(n + 1);
	QPointF R0;
	QPainter painter(widget);
	std::copy(points.begin(), points.end(), R.rbegin());
	for (int j = 0; j < Pointnum; j++) {
		for (m = n; m > 0; m--) {
			for (i = 0; i <= m - 1; i++) {
				Q[i].setX(R[i].x() + t * (R[i + 1].x() - R[i].x()));
				Q[i].setY(R[i].y() + t * (R[i + 1].y() - R[i].y()));
			}
			for (int i = 0; i <= m - 1; i++) {
				R[i] = Q[i];
			}
		}
		R0 = R[0];
		painter.drawPoint(R0);
		t += delta;
		R.clear();
		R.resize(n + 1);
		std::copy(points.begin(), points.end(), R.rbegin());
	}
	R.clear();
	Q.clear();
}

void Bazier_deCasteljau(QWidget* widget, vector<QPointF>& points, int nPoints) {
	int length = points.size();
	decase(widget, points, length - 1, nPoints);
}

vector<QPointF> ascend(QWidget* widget, vector<QPointF>& points) {
	int length = points.size();
	vector<QPointF> Q;
	Q.push_back(points[0]);
	for (int i = 1; i < length; i++) {
		double idnp1 = i / (double)(length);//½×Êýn=length-1
		QPointF p((1 - idnp1) * points[i].x() + idnp1 * points[i - 1].x(), (1 - idnp1) * points[i].y() + idnp1 * points[i - 1].y());
		Q.push_back(p);
	}
	Q.push_back(points[length - 1]);
	return Q;
}

#endif

