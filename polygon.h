#pragma once
#ifndef POLYGON_H
#define POLYGON_H

#include "vec2.h"
#include <vector>
#include <QWidget>
#include <QTextEdit>
#include <string>
#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <QPoint>

struct bucket {
	float ymax = -1;
	float xmin = -1;
	float one_m = -1;
	int direction;//1:up, -1:down
};

bool buckerComp(bucket b1, bucket b2) {
	if (b1.xmin != b2.xmin) {
		return b1.xmin < b2.xmin;
	}
	return b1.ymax < b2.ymax;
}

class EdgeTable {
public:
	vector<vector<bucket>> vvb;
	int ymax = 0;
	EdgeTable() {
		vvb = vector<vector<bucket>>(1280);
		for (int i = 0; i < 1280; i++) {
			vvb[i] = vector<bucket>();
		}
	}
	EdgeTable(int size) {
		vvb = vector<vector<bucket>>(size);
		for (int i = 0; i < size; i++) {
			vvb[i] = vector<bucket>();
		}
	}
};

EdgeTable buildEdgeTable(vector<vec2> points) {
	int size = points.size();
	int ymax = 0;
	for (int i = 0; i < size; i++) {
		ymax = max(ymax, int(points[i].y));
	}

	EdgeTable newTable(ymax + 1);
	newTable.ymax = ymax + 1;
	for (int i = 0; i < size; i++) {
		vec2 p1 = points[i], p2 = points[(i + 1) % size];
		bucket b;
		if (p1.y == p2.y) continue;

		float ymin = min(p1.y, p2.y);

		b.direction = 1;
		if (p1.y > p2.y) {
			std::swap(p1, p2);
			b.direction = -1;
		}
		//p1是下，p2是上

		b.ymax = p2.y;
		b.xmin = p1.x;
		b.one_m = (p2.x - p1.x) * 1.0f / (p2.y - p1.y);

		newTable.vvb[ymin].push_back(b);
	}
	return newTable;
}

void PolygonFill(QWidget* widget, vector<vec2>& points, bool isOddEven) {
	EdgeTable ET = buildEdgeTable(points);

	int y = 0;
	while (ET.vvb[y].size() == 0) {
		y++;
	}
	EdgeTable AET(1);

	bool* deletes = new bool[20];
	bool needToDelete = false;

	while (y < ET.ymax) {
		//insert new bucket into AET where y=y
		if (ET.vvb[y].size() != 0) {
			for (int i = 0; i < ET.vvb[y].size(); i++) {
				bucket b = ET.vvb[y][i];
				AET.vvb[0].push_back(b);
			}
		}

		//delete
		int length = AET.vvb[0].size();
		if (needToDelete && length != 0) {
			deletes = new bool[length];
			fill(deletes, deletes + length, false);
			for (int i = 0; i < length; i++) {
				if (AET.vvb[0][i].ymax <= y)
					deletes[i] = true;
			}
			vector<bucket> temp(length);
			std::copy(AET.vvb[0].begin(), AET.vvb[0].end(), temp.begin());
			AET.vvb[0].clear();
			for (int i = 0; i < length; i++) {
				if (!deletes[i])
					AET.vvb[0].push_back(temp[i]);
			}
			needToDelete = false;
		}

		//sort
		std::sort(AET.vvb[0].begin(), AET.vvb[0].end(), buckerComp);

		//draw points and plus x
		length = AET.vvb[0].size();
		if (isOddEven) {
			for (int i = 0; i < length; i += 2) {
				bucket b0 = AET.vvb[0][i];
				bucket b1 = AET.vvb[0][i + 1];
				for (int x = b0.xmin; x <= b1.xmin; x++) {
					drawPoint(widget, x, y);
				}
				AET.vvb[0][i].xmin += b0.one_m;
				AET.vvb[0][i + 1].xmin += b1.one_m;
				if (b0.ymax <= (y + 1)) needToDelete = true;
				if (b1.ymax <= (y + 1)) needToDelete = true;
			}
		}
		else {
			int i = 0;
			int sum = 0;
			int xleft, xright;
			while (i < length) {
				xleft = AET.vvb[0][i].xmin;
				do {
					xright = AET.vvb[0][i].xmin;
					sum += AET.vvb[0][i].direction;
					AET.vvb[0][i].xmin += AET.vvb[0][i].one_m;
					if (AET.vvb[0][i].ymax <= (y + 1)) needToDelete = true;
					i++;
				} while (sum != 0 && i < length);
				for (int x = xleft; x <= xright; x++) {
					drawPoint(widget, x, y);
				}
			}
		}

		//plus y
		y++;
	}
}

int findMiddestX(vector<vec2>& points) {
	int X = 0;
	int length = points.size();

	for (int i = 0; i < length; i++) {
		X += points[i].x;
	}

	X /= length;

	//return X;
	int res;
	int mindis = 100000;;
	for (int i = 0; i < length; i++) {
		int dis = abs(X - points[i].x);
		if (dis < mindis) {
			mindis = dis;
			res = points[i].x;
		}
	}
	return res;
}

void FencePolygonFill(QWidget* widget, vector<vec2>& points, bool isOddEven) {
	QRect rect = widget->frameGeometry();
	int width = rect.width();
	int height = rect.height();
	int** tempM = new int* [width];
	for (int i = 0; i < width; i++) {
		tempM[i] = new int[height];
		std::fill(tempM[i], tempM[i] + height, 0);
	}

	int fence = findMiddestX(points);
	int length = points.size();

	int minX = points[0].x, minY = points[0].y, maxX = minX, maxY = minY;

	for (int i = 0; i < length; i++) {
		point2 p0 = points[i];
		point2 p1 = points[(i + 1) % length];
		int direction = 1;
		if (p1.y < p0.y) direction = -1;

		if (p0.x < minX) minX = p0.x;
		if (p0.x > maxX) maxX = p0.x;
		if (p0.y < minY) minY = p0.y;
		if (p0.y > maxY) maxY = p0.y;

		if (p0.y == p1.y) continue;
		if (p0.y > p1.y) std::swap(p0, p1);
		float dx = (p1.x - p0.x) * 1.0f / (p1.y - p0.y);

		int y = p0.y;
		float x = p0.x;

		for (; y < p1.y; y++) {
			if (isOddEven) {
				if (x < fence) {
					for (int i = int(x + 1); i <= fence; i++) {
						tempM[i][y] = 1 - tempM[i][y];
					}
				}
				else {
					for (int i = fence + 1; i <= int(x); i++) {
						tempM[i][y] = 1 - tempM[i][y];
					}
				}
			}
			else {
				if (x < fence) {
					for (int i = int(x + 1); i <= fence; i++) {
						tempM[i][y] += direction;
					}
				}
				else {
					for (int i = fence + 1; i <= int(x); i++) {
						tempM[i][y] += direction;
					}
				}
			}
			x += dx;
		}
	}

	if (isOddEven) {
		for (int x = minX; x <= maxX; x++) {
			for (int y = minY; y < maxY; y++) {
				if (tempM[x][y])
					drawPoint(widget, x, y);
				//M.at<cv::Vec3b>(x, y) = cv::Vec3b(0, 0, 0);
			}
		}
	}
	else {
		for (int x = minX; x <= maxX; x++) {
			for (int y = minY; y < maxY; y++) {
				if (tempM[x][y] != 0)
					drawPoint(widget, x, y);
				//M.at<cv::Vec3b>(x, y) = cv::Vec3b(0, 0, 0);
			}
		}
	}
	return;
}

#endif

