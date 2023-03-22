#pragma once
#ifndef QTWIDGETSAPPLICATION4_H
#define QTWIDGETSAPPLICATION4_H

#include <QtWidgets/QWidget>
#include "ui_QtWidgetsApplication4.h"
#include <vector>
#include "vec2.h"

class Shapee;
class HomogeneousCoord;

enum PaintMode {
	Null = 0,
	Line,
	Circle,
	Ellipse,
	Triangulation_Prepare,
	Triangulation_Draw,
	FillPolygon,
	FillPolygonW,
	FenceFill,
	FenceFillW,
	ConvexHull_JarvisMarch,
	ConvexHull_DevideAndConquer,
	Triangulation_Zigzag,
	PolyInterCurve,
	CardinalCurve,
	BazierCurve,

	DrawRect,
	DrawHexagen,
	DrawStar,
	SetCenter,

	DragPointBazier,
};

class QtWidgetsApplication4 : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsApplication4(QWidget* parent = nullptr);
	~QtWidgetsApplication4();

private:
	Ui::QtWidgetsApplication4Class ui;

	std::vector<QPoint> points;
	std::vector<QPointF> fPoints;
	std::vector<vec2> tpoints;
	vector<pair<int, int>> segs;
	int pointIndex = 0;
	bool disableUpdate = false;
	float cardinalT = 0.5;
	bool needToDraw = true;

	vector<int> R;
	int circleIndex;

	HomogeneousCoord* center;
	Shapee* sshape = nullptr;
	Shapee* oldSshape = nullptr;
	PaintMode mode = PaintMode::Null;

	bool firstDrag = true;
	QPointF* selectedPointBazier = nullptr;
	QPointF* tempPointF = nullptr;

	double findRadius = 25;
	bool findClosestPointF(QPointF& p);

	bool rotateControl = false;
	bool scaleControl = false;
	bool transformControl = false;
	bool shearControl = false;
	int rhelp = 0;
	double originLength = 0.0f;
	double targetLength = 0.0f;
	double originX = 0.0f;
	double originY = 0.0f;
	bool shearXAxis = true;
	QPointF originOrientation;
	QPointF targetOrientation;

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void paintEvent(QPaintEvent* e);
	void keyPressEvent(QKeyEvent*);

private slots:
	void on_DrawLineBtn_clicked();
	void on_TriangleBtn_clicked();
	void on_OKBtn_clicked();
	void on_SaveTextBtn_clicked();
	void on_disableUpdate_clicked();
	void on_PolyInterCurve_clicked();
	void on_CardinalCurve_clicked();
	void on_BezierBtn_clicked();
	void on_ConvexHullJ_clicked();
	void on_ConvexHullDC_clicked();
	void on_TriangulationZ_clicked();
	void on_FillPolygon_clicked();
	void on_DrawCircle_clicked();
	void on_DrawEllipse_clicked();
	void on_FillPolygonW_clicked();
	void on_FenceFill_clicked();
	void on_FenceFillW_clicked();
	void on_DrawRec_clicked();
	void on_TransormBtn_clicked();
	void on_RotateBtn_clicked();
	void on_ScaleBtn_clicked();
	void on_DrawHexagen_clicked();
	void on_DrawStar_clicked();
	void on_SetRotateCenter_clicked();
	void on_SetCenter_clicked();
	void on_Shear_clicked();
	void on_BazierAscend_clicked();
	void on_DragPoint_clicked();
	void on_AddPoint_clicked();
};

#endif
