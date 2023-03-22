#pragma once
#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <iostream>
#include <QtWidgets/QWidget>
#include <QPainter>
using namespace std;

enum COLOR {
	black = 0, blue = 1, green = 2,
};

inline void drawDottedPoint(QWidget* qwidget, float x, float y, int& j) {
	if ((j++) % 4 <= 1) {
		QPainter painter(qwidget);
		painter.setPen(QPen(QColor(0, 0, 255), 1));
		painter.drawPoint(QPoint(x, y));
	}
}

inline void drawPoint(QWidget* qwidget, float x, float y) {
	QPainter painter(qwidget);
	painter.drawPoint(QPoint(x, y));
}

inline void drawPoint_t(QWidget* qwidget, float x, float y) {
	QPainter painter(qwidget);
	painter.drawPoint(QPoint(y, x));
}

void DottedBresenhamLine(QWidget* qwidget, int x1, int y1, int x2, int y2) {
	int x, y, dx, dy, p;
	int j;


	if (x1 == x2) {
		if (y1 > y2) {
			std::swap(y1, y2);
		}
		for (int y = y1; y <= y2; y++) {
			drawDottedPoint(qwidget, x1, y, j);
		}
		return;
	}

	if (y1 == y2) {
		if (x1 > x2) {
			std::swap(x1, x2);
		}
		for (int x = x1; x <= x2; x++) {
			drawDottedPoint(qwidget, x, y1, j);
		}
	}

	if ((x1 > x2 && y1 > y2) || (x1 > x2 && y1 < y2 && (x1 - x2 > y2 - y1)) || (x2 > x1 && y2 < y1 && (y1 - y2 > x2 - x1))) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (0 < dy && dy <= dx) {
		p = dy + dy - dx;
		for (; x < x2; x++) {
			drawDottedPoint(qwidget, x, y, j);
			if (p >= 0) {
				y++;
				p += ((dy - dx) + (dy - dx));//+2dy -2dx
			}
			else {
				p += (dy + dy);          //+2dy
			}
		}
	}
	else if (0 < dx && dx <= dy) {
		p = dx + dx - dy;
		for (; y < y2; y++) {
			drawDottedPoint(qwidget, x, y, j);
			if (p >= 0) {
				x++;
				p += ((dx - dy) + (dx - dy));
			}
			else {
				p += (dx + dx);
			}
		}
	}
	else if (dy < 0 && dx > 0 && (-dy) <= dx) {
		dy = -dy;
		p = dy + dy - dx;
		for (; x < x2; x++) {
			drawDottedPoint(qwidget, x, y, j);
			if (p >= 0) {
				y--;
				p += ((dy - dx) + (dy - dx));
			}
			else {
				p += (dy + dy);
			}
		}
	}
	else if (dx < 0 && dy > 0 && (-dx) <= dy) {
		dx = -dx;
		p = dx + dx - dy;
		for (; y < y2; y++) {
			drawDottedPoint(qwidget, x, y, j);
			if (p >= 0) {
				x--;
				p += ((dx - dy) + (dx - dy));
			}
			else {
				p += (dx + dx);
			}
		}
	}
}


void BresenhamLine(QWidget* qwidget, int x1, int y1, int x2, int y2) {
	//x1 *= 5;
	//x2 *= 5;
	//y1 *= 5;
	//y2 *= 5;

	int x, y, dx, dy, p;

	if (x1 == x2) {
		if (y1 > y2) {
			std::swap(y1, y2);
		}
		for (int y = y1; y <= y2; y++) {
			drawPoint(qwidget, x1, y);
		}
		return;
	}

	if (y1 == y2) {
		if (x1 > x2) {
			std::swap(x1, x2);
		}
		for (int x = x1; x <= x2; x++) {
			drawPoint(qwidget, x, y1);
		}
	}

	if ((x1 > x2 && y1 > y2) || (x1 > x2 && y1 < y2 && (x1 - x2 > y2 - y1)) || (x2 > x1 && y2 < y1 && (y1 - y2 > x2 - x1))) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (0 < dy && dy <= dx) {
		p = dy + dy - dx;
		for (; x < x2; x++) {
			drawPoint(qwidget, x, y);
			if (p >= 0) {
				y++;
				p += ((dy - dx) + (dy - dx));//+2dy -2dx
			}
			else {
				p += (dy + dy);          //+2dy
			}
		}
	}
	else if (0 < dx && dx <= dy) {
		p = dx + dx - dy;
		for (; y < y2; y++) {
			drawPoint(qwidget, x, y);
			if (p >= 0) {
				x++;
				p += ((dx - dy) + (dx - dy));
			}
			else {
				p += (dx + dx);
			}
		}
	}
	else if (dy < 0 && dx > 0 && (-dy) <= dx) {
		dy = -dy;
		p = dy + dy - dx;
		for (; x < x2; x++) {
			drawPoint(qwidget, x, y);
			if (p >= 0) {
				y--;
				p += ((dy - dx) + (dy - dx));
			}
			else {
				p += (dy + dy);
			}
		}
	}
	else if (dx < 0 && dy > 0 && (-dx) <= dy) {
		dx = -dx;
		p = dx + dx - dy;
		for (; y < y2; y++) {
			drawPoint(qwidget, x, y);
			if (p >= 0) {
				x--;
				p += ((dx - dy) + (dx - dy));
			}
			else {
				p += (dx + dx);
			}
		}
	}
}

void BresenhamLine(QWidget* qwidget, QPoint p1, QPoint p2) {
	BresenhamLine(qwidget, p1.x(), p1.y(), p2.x(), p2.y());
}

void DrawBigPoint(QWidget* qwidget, float x, float y, COLOR c = black) {
	QPainter painter(qwidget);
	if (c == COLOR::blue) {
		painter.setPen(QPen(QColor(0, 0, 255), 1));
	}
	else if (c == COLOR::green) {
		painter.setPen(QPen(QColor(0, 255, 0), 1));
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			painter.drawPoint(x + 2 - i, y + 2 - j);
		}
	}
	painter.setPen(QPen(QColor(0, 0, 0), 1));
}

void DrawBigPoint(QWidget* qwidget, QPoint p) {
	DrawBigPoint(qwidget, p.x(), p.y());
}

void DrawSymmetricPointCircle(QWidget* qwidget, int startx, int starty, int x, int y) {
	drawPoint(qwidget, startx + x, starty + y);
	drawPoint(qwidget, startx + y, starty + x);
	drawPoint(qwidget, startx - x, starty + y);
	drawPoint(qwidget, startx - y, starty + x);
	drawPoint(qwidget, startx - x, starty - y);
	drawPoint(qwidget, startx - y, starty - x);
	drawPoint(qwidget, startx + x, starty - y);
	drawPoint(qwidget, startx + y, starty - x);
}

void BresenhamCircle(QWidget* qwidget, int R, int centerx, int centery) {
	int x, y, p;
	x = 0;
	y = R;
	p = 3 - 2 * R;
	for (; x <= y; x++) {
		DrawSymmetricPointCircle(qwidget, centerx, centery, x, y);
		//drawPoint(qwidget, x, y);
		if (p >= 0) {
			p += 4 * (x - y) + 10;
			y--;
		}
		else {
			p += 4 * x + 6;
		}
	}
}

void DrawSymmetricPointEllipse(QWidget* qwidget, int startx, int starty, int x, int y) {
	drawPoint(qwidget, startx + x, starty + y);
	drawPoint(qwidget, startx + x, starty - y);
	drawPoint(qwidget, startx - x, starty - y);
	drawPoint(qwidget, startx - x, starty + y);
}

void BresenhamEllipse(QWidget* qwidget, int centerx, int centery, int a, int b) {
	int x, y, p, bSquare, aSquare;
	x = 0;
	y = b;
	bSquare = b * b;
	aSquare = a * a;
	p = 2 * bSquare - 2 * aSquare * b + aSquare;
	for (; bSquare * x <= aSquare * y; x++) {
		DrawSymmetricPointEllipse(qwidget, centerx, centery, x, y);
		if (p >= 0) {
			p += 4 * (bSquare * x - aSquare * y) + 6 * bSquare + 4 * aSquare;
			y--;
		}
		else {
			p += 4 * bSquare * x + 6 * bSquare;
		}
	}

	x = a;
	y = 0;
	p = 2 * aSquare + bSquare - 2 * a * bSquare;
	for (; bSquare * x >= aSquare * y; y++) {
		DrawSymmetricPointEllipse(qwidget, centerx, centery, x, y);
		if (p >= 0) {
			p += 4 * (aSquare * y - bSquare * x) + 6 * aSquare + 4 * bSquare;
			x--;
		}
		else {
			p += 4 * aSquare * y + 6 * aSquare;
		}
	}
}

#endif


