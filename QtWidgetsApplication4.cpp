#include "QtWidgetsApplication4.h"
//#include <QPaintEvent>
//#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTextEdit>
#include <QFile>
#include <QMessageBox>
#include "TriangleDevision.h"
#include "transform.h"
#include "curve.h"
#include "ConvexHull.h"
#include "Triangulation.h"
#include "polygon.h"


QtWidgetsApplication4::QtWidgetsApplication4(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.TransformControl->setEnabled(false);

	connect(ui.DrawLineBtn, &QPushButton::clicked, this, &QtWidgetsApplication4::on_DrawLineBtn_clicked);
	connect(ui.TriangleBtn, &QPushButton::clicked, this, &QtWidgetsApplication4::on_TriangleBtn_clicked);
	connect(ui.OKBtn, &QPushButton::clicked, this, &QtWidgetsApplication4::on_OKBtn_clicked);
	connect(ui.SaveTextBtn, &QPushButton::clicked, this, &QtWidgetsApplication4::on_SaveTextBtn_clicked);
	connect(ui.DisableUpdate, &QPushButton::clicked, this, &QtWidgetsApplication4::on_disableUpdate_clicked);
}

QtWidgetsApplication4::~QtWidgetsApplication4()
{}

bool QtWidgetsApplication4::findClosestPointF(QPointF& p)
{
	int length = fPoints.size();
	if (length == 0) return false;
	double minDis = findRadius + 0.0001;
	for (int i = 0; i < length; i++) {
		double dx = p.x() - fPoints[i].x();
		double dy = p.y() - fPoints[i].y();
		double dis = sqrt(dx * dx + dy * dy);
		if (dis < minDis) {
			minDis = dis;
			selectedPointBazier = &fPoints[i];
		}
	}
	if (minDis <= findRadius) {
		return true;
	}
	else {
		return false;
	}
}

void QtWidgetsApplication4::mousePressEvent(QMouseEvent* e) {
	if (mode == PaintMode::Line) {
		QPoint newP(e->x(), e->y());
		points.push_back(newP);
	}
	else if (mode == PaintMode::CardinalCurve || mode == PaintMode::PolyInterCurve) {
		QPoint newP(e->x(), e->y());
		points.push_back(newP);
	}
	else if (mode == PaintMode::BazierCurve) {
		QPointF newP(e->x(), e->y());
		fPoints.push_back(newP);
	}
	else if (mode == PaintMode::ConvexHull_DevideAndConquer || mode == PaintMode::ConvexHull_JarvisMarch || mode == PaintMode::Triangulation_Zigzag || mode == PaintMode::FillPolygon || mode == PaintMode::FillPolygonW || mode == PaintMode::FenceFill || mode == PaintMode::FenceFillW) {
		point2 newP(e->x(), e->y());
		tpoints.push_back(newP);
	}
	else if (mode == PaintMode::Triangulation_Prepare) {
		vec2 point(e->x(), e->y(), pointIndex++);
		tpoints.push_back(point);
		ui.tPointText->append(tr("(%1, %2)").arg(e->x()).arg(e->y()));
	}
	else if (mode == PaintMode::Circle) {
		if (circleIndex % 2 == 0) {
			QPoint circleCenter = QPoint(e->x(), e->y());
			points.push_back(circleCenter);
		}
		else if (circleIndex % 2 == 1) {
			QPoint p = QPoint(e->x(), e->y());
			QPoint last;
			p.setX(p.x() - points[points.size() - 1].x());
			p.setY(p.y() - points[points.size() - 1].y());
			R.push_back(sqrt(p.x() * p.x() + p.y() * p.y()));
		}
		circleIndex++;
	}
	else if (mode == PaintMode::Ellipse) {
		if (circleIndex % 2 == 0) {
			QPoint ellipseLeftUp = QPoint(e->x(), e->y());
			points.push_back(ellipseLeftUp);
		}
		else if (circleIndex % 2 == 1) {
			QPoint ellipseRightDown = QPoint(e->x(), e->y());
			if ((ellipseRightDown.x() - points[points.size() - 1].x()) % 2 == 1) ellipseRightDown.setX(ellipseRightDown.x() + 1);
			if ((ellipseRightDown.y() - points[points.size() - 1].y()) % 2 == 1) ellipseRightDown.setY(ellipseRightDown.y() + 1);
			points.push_back(ellipseRightDown);
		}
		circleIndex++;
	}
	else if (mode == PaintMode::DrawRect) {
		if (circleIndex == 0) {
			QPoint first = QPoint(e->x(), e->y());
			points.push_back(first);
			setMouseTracking(true);
		}
		else if (circleIndex == 1) {
			QPoint second = QPoint(e->x(), e->y());
			if (sshape != nullptr) {
				delete sshape;
			}
			this->sshape = new RegularRect(HomogeneousCoord(points[0].x(), points[0].y()), HomogeneousCoord(second.x(), second.y()));
			setMouseTracking(false);
		}
		circleIndex++;
	}
	else if (mode == PaintMode::DrawHexagen) {
		if (circleIndex == 0) {
			QPoint first = QPoint(e->x(), e->y());
			points.push_back(first);
			setMouseTracking(true);
		}
		else if (circleIndex == 1) {
			//QPoint second = QPoint(e->x(), e->y());
			if (sshape != nullptr) {
				delete sshape;
			}
			this->sshape = new Hexagon(points[0].x(), points[0].y(), e->x(), e->y());
			setMouseTracking(false);
		}
		circleIndex++;
	}
	else if (mode == PaintMode::DrawStar) {
		if (circleIndex == 0) {
			QPoint first = QPoint(e->x(), e->y());
			points.push_back(first);
			setMouseTracking(true);
		}
		else if (circleIndex == 1) {
			//QPoint second = QPoint(e->x(), e->y());
			if (sshape != nullptr) {
				delete sshape;
			}
			this->sshape = new RegularStar(points[0], QPoint(e->x(), e->y()));
			setMouseTracking(false);
		}
		circleIndex++;
	}
	else if (mode == PaintMode::SetCenter) {
		center = new HomogeneousCoord(e->x(), e->y());
	}

	needToDraw = true;
	update();
}

int ooo = 0;

void QtWidgetsApplication4::mouseMoveEvent(QMouseEvent* e) {
	if (mode == PaintMode::DragPointBazier) {
		if (firstDrag) {
			QPointF mousePoint(e->x(), e->y());
			if (findClosestPointF(mousePoint)) {
				if (selectedPointBazier == nullptr) return;
				firstDrag = false;
				return;
			}
		}
		if (selectedPointBazier != nullptr) {
			//ui.tPointText->setText(QString::fromStdString(std::string("mouseMove" + to_string(ooo++))));
			if (tempPointF == nullptr) {
				tempPointF = new QPointF(e->x(), e->y());
			}
			else {
				tempPointF->setX(e->x());
				tempPointF->setY(e->y());
			}
			repaint();
		}
	}
	else if (mode == PaintMode::DrawRect && circleIndex == 1) {
		ui.tPointText->setText(QString::fromStdString(std::string("circleIndex" + to_string(circleIndex))));
		HomogeneousCoord h(e->x(), e->y());
		if (sshape != nullptr) {
			delete sshape;
		}
		sshape = new RegularRect(HomogeneousCoord(points[0].x(), points[0].y()), h);
		repaint();
	}
	else if (mode == PaintMode::DrawStar && circleIndex == 1) {
		ui.tPointText->setText(QString::fromStdString(std::string("circleIndex" + to_string(circleIndex))));
		HomogeneousCoord h(e->x(), e->y());
		if (sshape != nullptr) {
			delete sshape;
		}
		sshape = new RegularStar(HomogeneousCoord(points[0].x(), points[0].y()), h);
		repaint();
	}
	else if (mode == PaintMode::DrawHexagen && circleIndex == 1) {
		ui.tPointText->setText(QString::fromStdString(std::string("DrawHexagen" + to_string(circleIndex))));
		HomogeneousCoord h(e->x(), e->y());
		if (sshape != nullptr) {
			delete sshape;
		}
		//sshape = new Hexagon(points[0].x(), points[0].y(), h.x(), h.y());
		sshape = new Hexagon(HomogeneousCoord(points[0].x(), points[0].y()), h);
		repaint();
	}
	else if (mode == PaintMode::DrawStar || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawRect || mode == PaintMode::SetCenter) {
		if (rotateControl && sshape != nullptr) {
			ui.tPointText->setText(QString::fromStdString(std::string("mouseMove" + to_string(ooo++))));
			if (rhelp == 0) {
				rhelp = 1;
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				double length = sqrt(x * x + y * y);
				originOrientation = QPointF(x / length, y / length);
				oldSshape = sshape->copy();
			}
			else {
				if (tempPointF == nullptr) {
					tempPointF = new QPointF(e->x(), e->y());
				}
				else {
					tempPointF->setX(e->x());
					tempPointF->setY(e->y());
				}
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				double length = sqrt(x * x + y * y);
				targetOrientation = QPointF(x / length, y / length);
				double costheta = originOrientation.x() * targetOrientation.x() + originOrientation.y() * targetOrientation.y();
				bool orientation = (originOrientation.x() * targetOrientation.y() - originOrientation.y() * targetOrientation.x()) >= 0;
				double theta = acos(costheta);
				theta = theta * 180.0 / pi;
				if (!orientation) {
					theta = -theta;
				}
				ui.tPointText->setText(QString::fromStdString(std::to_string(theta)));
				delete sshape;
				sshape = oldSshape->copy();
				sshape->rotate(theta, HomogeneousCoord(center->x(), center->y()));
				repaint();
			}
		}
		else if (scaleControl && sshape != nullptr) {
			if (rhelp == 0) {
				rhelp = 1;
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				originLength = sqrt(x * x + y * y);
				oldSshape = sshape->copy();
			}
			else {
				if (tempPointF == nullptr) {
					tempPointF = new QPointF(e->x(), e->y());
				}
				else {
					tempPointF->setX(e->x());
					tempPointF->setY(e->y());
				}
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				double targetLength = sqrt(x * x + y * y);
				double scaleRatio = targetLength / originLength;
				ui.tPointText->setText(QString::fromStdString(std::to_string(scaleRatio)));
				delete sshape;
				sshape = oldSshape->copy();
				sshape->scale(scaleRatio, scaleRatio, *center);
				repaint();
			}
		}
		else if (transformControl && sshape != nullptr) {
			if (rhelp == 0) {
				rhelp = 1;
				oldSshape = sshape->copy();
				originX = e->x();
				originY = e->y();
			}
			else {
				if (tempPointF == nullptr) {
					tempPointF = new QPointF(e->x(), e->y());
				}
				else {
					tempPointF->setX(e->x());
					tempPointF->setY(e->y());
				}
				double x = e->x() - originX;
				double y = e->y() - originY;
				ui.tPointText->setText(QString::fromStdString("x:" + std::to_string(x) + ",y:" + std::to_string(y)));
				delete sshape;
				sshape = oldSshape->copy();
				sshape->transform(x, y);
				repaint();
			}
		}
		else if (shearControl && sshape != nullptr) {
			ui.tPointText->setText(QString::fromStdString(std::string("mouseMove" + to_string(ooo++))));
			if (rhelp == 0) {
				rhelp = 1;
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				double length = sqrt(x * x + y * y);
				originOrientation = QPointF(x / length, y / length);
				oldSshape = sshape->copy();
			}
			else {
				if (tempPointF == nullptr) {
					tempPointF = new QPointF(e->x(), e->y());
				}
				else {
					tempPointF->setX(e->x());
					tempPointF->setY(e->y());
				}
				double x = e->x() - center->x();
				double y = e->y() - center->y();
				double length = sqrt(x * x + y * y);
				targetOrientation = QPointF(x / length, y / length);
				double costheta = originOrientation.x() * targetOrientation.x() + originOrientation.y() * targetOrientation.y();
				bool orientation = (originOrientation.x() * targetOrientation.y() - originOrientation.y() * targetOrientation.x()) >= 0;
				double theta = acos(costheta);
				theta = theta * 180.0 / pi;
				if (!orientation) {
					theta = -theta;
				}
				ui.tPointText->setText(QString::fromStdString(std::to_string(theta)));
				delete sshape;
				sshape = oldSshape->copy();
				sshape->stear(theta, shearXAxis);
				repaint();
			}
		}
	}
}

void QtWidgetsApplication4::mouseReleaseEvent(QMouseEvent* e) {
	if (mode == PaintMode::DragPointBazier && selectedPointBazier != nullptr) {
		selectedPointBazier->setX(e->x());
		selectedPointBazier->setY(e->y());
		selectedPointBazier = nullptr;
		tempPointF = nullptr;
		firstDrag = true;
		repaint();
	}
}

void QtWidgetsApplication4::paintEvent(QPaintEvent* e) {
	if (needToDraw) {
		if (mode == PaintMode::Line) {
			//BresenhamLine(this, 0, 0, 100, 100);
			int length = points.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, points[i]);
			}
			for (int i = 0; i < length - 1; i++) {
				BresenhamLine(this, points[i], points[i + 1]);
			}
		}
		else if (mode == PaintMode::Triangulation_Prepare) {
			int tlength = tpoints.size();
			for (int i = 0; i < tlength; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
		}
		else if (mode == PaintMode::Triangulation_Draw) {
			int tlength = tpoints.size();
			int slength = segs.size();

			for (int i = 0; i < tlength - 1; i++) {
				BresenhamLine(this, tpoints[i].x, tpoints[i].y, tpoints[i + 1].x, tpoints[i + 1].y);
			}
			BresenhamLine(this, tpoints[tlength - 1].x, tpoints[tlength - 1].y, tpoints[0].x, tpoints[0].y);

			for (auto it : segs) {
				BresenhamLine(this, tpoints[it.first].x, tpoints[it.first].y, tpoints[it.second].x, tpoints[it.second].y);
			}

			for (int i = 0; i < tlength; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
		}
		else if (mode == PaintMode::PolyInterCurve) {
			LagrangeCurve(this, points);
		}
		else if (mode == PaintMode::CardinalCurve) {
			int length = points.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, points[i].x(), points[i].y());
			}
			if (points.size() > 3) {
				CardCurveAll(this, points, cardinalT);
			}
		}
		else if (mode == PaintMode::BazierCurve || mode == PaintMode::DragPointBazier) {
			int length = fPoints.size();
			for (int i = 0; i < length; i++) {
				if (selectedPointBazier != nullptr && selectedPointBazier == &fPoints[i]) {
					DrawBigPoint(this, fPoints[i].x(), fPoints[i].y(), COLOR::green);
				}
				else {
					DrawBigPoint(this, fPoints[i].x(), fPoints[i].y());
				}
			}
			if (tempPointF != nullptr) {
				DrawBigPoint(this, tempPointF->x(), tempPointF->y(), COLOR::green);
			}
			if (length > 3) {
				//Bazier(this, points, cardinalT);
				Bazier_deCasteljau(this, fPoints, length * 200);
			}
		}
		else if (mode == PaintMode::ConvexHull_DevideAndConquer || mode == PaintMode::ConvexHull_JarvisMarch) {
			int length = tpoints.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}

			if (length >= 3) {
				sort(tpoints.begin(), tpoints.end(), ComparatorXmin);
				vector<point2> convexHull;
				if (mode == PaintMode::ConvexHull_JarvisMarch) {
					convexHull = getConvexHull_n2_2(tpoints);
					int ltl = LowestthenLeftmost(convexHull);
					vltl = convexHull[ltl];
					if (ltl != 0) {
						std::swap(convexHull[0], convexHull[ltl]);
					}
					sort(convexHull.begin(), convexHull.end(), comp2);
				}
				else if (mode == PaintMode::ConvexHull_DevideAndConquer) {
					convexHull = zigzag(tpoints);
				}
				length = convexHull.size();
				for (int i = 0; i < length - 1; i++) {
					BresenhamLine(this, convexHull[i].x, convexHull[i].y, convexHull[i + 1].x, convexHull[i + 1].y);
				}
				BresenhamLine(this, convexHull[length - 1].x, convexHull[length - 1].y, convexHull[0].x, convexHull[0].y);
			}
		}
		else if (mode == PaintMode::Triangulation_Zigzag) {
			int length = tpoints.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
			if (length >= 3) {
				sort(tpoints.begin(), tpoints.end(), ComparatorXmin);
				vector<pair<point2, point2>> segs;
				segs = zigzagTriangulation(tpoints);
				length = segs.size();
				for (int i = 0; i < length; i++) {
					BresenhamLine(this, segs[i].first.x, segs[i].first.y, segs[i].second.x, segs[i].second.y);
				}
			}
		}
		else if (mode == PaintMode::FillPolygon || mode == PaintMode::FillPolygonW) {
			int length = tpoints.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
			for (int i = 0; i < length; i++) {
				BresenhamLine(this, tpoints[i].x, tpoints[i].y, tpoints[(i + 1) % length].x, tpoints[(i + 1) % length].y);
			}
			if (length >= 3) {
				PolygonFill(this, tpoints, 7 - mode);
			}
		}
		else if (mode == PaintMode::Circle) {
			int Rlength = R.size();
			for (int i = 0; i < Rlength; i++) {
				QPoint circleCenter = points[i];
				int r = R[i];
				DrawBigPoint(this, circleCenter.x(), circleCenter.y());
				BresenhamCircle(this, r, circleCenter.x(), circleCenter.y());
			}
			if (circleIndex % 2 == 1) {
				QPoint circleCenter = points[Rlength];
				DrawBigPoint(this, circleCenter.x(), circleCenter.y());
			}
			needToDraw = false;
		}
		else if (mode == PaintMode::Ellipse) {
			int length = points.size() / 2;
			for (int i = 0; i < length * 2; i += 2) {
				QPoint ellipseLeftUp = points[i];
				QPoint ellipseRightDown = points[i + 1];
				DrawBigPoint(this, ellipseLeftUp);
				DrawBigPoint(this, ellipseRightDown);

				QPoint center = (ellipseLeftUp + ellipseRightDown) / 2;
				int a = (ellipseRightDown.x() - ellipseLeftUp.x()) / 2;
				int b = (ellipseRightDown.y() - ellipseLeftUp.y()) / 2;

				if (a < 0) a *= -1;
				if (b < 0) b *= -1;

				BresenhamEllipse(this, center.x(), center.y(), a, b);
			}
			if (points.size() % 2 == 1) {
				DrawBigPoint(this, points[length * 2]);
			}
			needToDraw = false;
		}
		else if (mode == PaintMode::FenceFill) {
			int length = tpoints.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
			if (length >= 3) {
				FencePolygonFill(this, tpoints, true);
			}
			needToDraw = false;
		}
		else if (mode == PaintMode::FenceFillW) {
			int length = tpoints.size();
			for (int i = 0; i < length; i++) {
				DrawBigPoint(this, tpoints[i].x, tpoints[i].y);
			}
			if (length >= 3) {
				FencePolygonFill(this, tpoints, false);
			}
			needToDraw = false;
		}
		else if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
			DrawBigPoint(this, center->x(), center->y());
			if (circleIndex == 1) {
				DrawBigPoint(this, points[0]);
			}
			if (sshape != nullptr) {
				sshape->draw(this);
			}

			if ((rotateControl || scaleControl || shearControl) && center != nullptr && tempPointF != nullptr) {
				DottedBresenhamLine(this, center->x(), center->y(), tempPointF->x(), tempPointF->y());
			}
		}
	}
}

void QtWidgetsApplication4::keyPressEvent(QKeyEvent* e)
{
	if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
		if (e->key() == Qt::Key_R) {
			ui.tPointText->setText("R");
			rotateControl = 1 - rotateControl;
			this->setMouseTracking(rotateControl);
			ui.TransformControl->setMouseTracking(rotateControl);
			repaint();
			rhelp = 0;
		}
		else if (e->key() == Qt::Key_S) {
			ui.tPointText->setText("S");
			scaleControl = 1 - scaleControl;
			this->setMouseTracking(scaleControl);
			ui.TransformControl->setMouseTracking(scaleControl);
			repaint();
			rhelp = 0;
		}
		else if (e->key() == Qt::Key_T) {
			ui.tPointText->setText("T");
			transformControl = 1 - transformControl;
			this->setMouseTracking(transformControl);
			ui.TransformControl->setMouseTracking(transformControl);
			repaint();
			rhelp = 0;
		}
		else if (e->key() == Qt::Key_H) {
			ui.tPointText->setText("H");
			shearControl = 1 - shearControl;
			this->setMouseTracking(shearControl);
			ui.TransformControl->setMouseTracking(shearControl);
			repaint();
			rhelp = 0;
		}
		else if (e->key() == Qt::Key_X) {
			if (shearControl) {
				shearXAxis = true;
				repaint();
			}
		}
		else if (e->key() == Qt::Key_Y) {
			if (shearControl) {
				shearXAxis = false;
				repaint();
			}
		}
	}
}

void QtWidgetsApplication4::on_DrawLineBtn_clicked()
{
	mode = PaintMode::Line;
	points.clear();
	needToDraw = true;
	update();
}


void QtWidgetsApplication4::on_TriangleBtn_clicked()
{
	mode = PaintMode::Triangulation_Prepare;
	tpoints.clear();
	pointIndex = 0;
	ui.tPointText->clear();
	needToDraw = true;
	update();
}


void QtWidgetsApplication4::on_OKBtn_clicked()
{
	if (mode == Triangulation_Prepare) {
		mode = PaintMode::Triangulation_Draw;
	}
	DevideIntoTriangle(tpoints, segs);
	needToDraw = true;
	update();
}


void QtWidgetsApplication4::on_SaveTextBtn_clicked()
{
	QString fileName = "TrianglationTest";
	QFile file(fileName);

	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("多文档编辑器"), tr("无法写入文件%1:/n %2").arg(fileName).arg(file.errorString()));
		return;
	}
	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << ui.tPointText->toPlainText();
	QApplication::restoreOverrideCursor();
	return;
}

void QtWidgetsApplication4::on_disableUpdate_clicked()
{
	disableUpdate = 1 - disableUpdate;
	if (disableUpdate) {
		ui.DisableUpdate->setText(tr("AbleUpdate"));
		setUpdatesEnabled(false);
	}
	else {
		setUpdatesEnabled(true);
		ui.DisableUpdate->setText(tr("DisableUpdate"));
	}
	needToDraw = true;
}


void QtWidgetsApplication4::on_PolyInterCurve_clicked()
{
	mode = PaintMode::PolyInterCurve;
	points.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_CardinalCurve_clicked()
{
	mode = PaintMode::CardinalCurve;
	points.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_BezierBtn_clicked()
{
	mode = PaintMode::BazierCurve;
	fPoints.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_ConvexHullJ_clicked()
{
	mode = PaintMode::ConvexHull_JarvisMarch;
	tpoints.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_ConvexHullDC_clicked()
{
	mode = PaintMode::ConvexHull_DevideAndConquer;
	tpoints.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_TriangulationZ_clicked()
{
	mode = PaintMode::Triangulation_Zigzag;
	tpoints.clear();
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_FillPolygon_clicked()
{
	if (mode != PaintMode::FillPolygonW) {
		tpoints.clear();
	}
	mode = PaintMode::FillPolygon;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}

void QtWidgetsApplication4::on_FillPolygonW_clicked()
{
	if (mode != PaintMode::FillPolygon) {
		tpoints.clear();
	}
	mode = PaintMode::FillPolygonW;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}

void QtWidgetsApplication4::on_DrawCircle_clicked()
{
	mode = PaintMode::Circle;
	points.clear();
	R.clear();
	circleIndex = 0;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_DrawEllipse_clicked()
{
	mode = PaintMode::Ellipse;
	points.clear();
	circleIndex = 0;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_FenceFill_clicked()
{
	if (mode != PaintMode::FenceFillW) {
		tpoints.clear();
	}
	mode = PaintMode::FenceFill;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_FenceFillW_clicked()
{
	if (mode != PaintMode::FenceFill) {
		tpoints.clear();
	}
	mode = PaintMode::FenceFillW;
	update();
	needToDraw = true;
	ui.TransformControl->setEnabled(false);
}


void QtWidgetsApplication4::on_DrawRec_clicked()
{
	mode = PaintMode::DrawRect;
	this->rotateControl = false;
	this->setMouseTracking(rotateControl);
	this->ui.TransformControl->setMouseTracking(rotateControl);

	ui.TransformControl->setEnabled(true);
	circleIndex = 0;
	this->sshape = nullptr;
	center = new HomogeneousCoord(0, 0);
	points.clear();
	update();
	needToDraw = true;
}



void QtWidgetsApplication4::on_TransormBtn_clicked()
{
	if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
		if (this->sshape == nullptr) {
			ui.tPointText->setText("the pattern has not been created yet!");
			return;
		}
		float _x = ui.transformTextx->toPlainText().toFloat();
		float _y = ui.transformTexty->toPlainText().toFloat();

		this->sshape->transform(_x, _y);
		repaint();
	}
}


void QtWidgetsApplication4::on_RotateBtn_clicked()
{
	if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
		if (this->sshape == nullptr) {
			ui.tPointText->setText("the pattern has not been created yet!");
			return;
		}
		float angle = ui.rotateText->toPlainText().toFloat();

		this->sshape->rotate(angle, *center);
		repaint();
	}
}


void QtWidgetsApplication4::on_ScaleBtn_clicked()
{
	if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
		if (this->sshape == nullptr) {
			ui.tPointText->setText("the pattern has not been created yet!");
			return;
		}
		float _x = ui.transformTextx->toPlainText().toFloat();
		float _y = ui.transformTexty->toPlainText().toFloat();

		this->sshape->scale(_x, _y, *center);
		repaint();
	}
}


void QtWidgetsApplication4::on_DrawHexagen_clicked()
{
	mode = PaintMode::DrawHexagen;
	this->rotateControl = false;
	this->setMouseTracking(rotateControl);
	this->ui.TransformControl->setMouseTracking(rotateControl);

	ui.TransformControl->setEnabled(true);
	circleIndex = 0;
	this->sshape = nullptr;
	points.clear();
	center = new HomogeneousCoord(0, 0);
	update();
	needToDraw = true;
}


void QtWidgetsApplication4::on_DrawStar_clicked()
{
	mode = PaintMode::DrawStar;
	this->rotateControl = false;
	this->setMouseTracking(rotateControl);
	this->ui.TransformControl->setMouseTracking(rotateControl);

	ui.TransformControl->setEnabled(true);
	circleIndex = 0;
	this->sshape = nullptr;
	center = new HomogeneousCoord(0, 0);
	points.clear();
	update();
	needToDraw = true;
}


void QtWidgetsApplication4::on_SetRotateCenter_clicked()
{
	mode = PaintMode::SetCenter;
	ui.TransformControl->setEnabled(true);
}



void QtWidgetsApplication4::on_SetCenter_clicked()
{
	if (sshape != nullptr) {
		center = sshape->center();
		repaint();
	}
}


void QtWidgetsApplication4::on_Shear_clicked()
{
	if (mode == PaintMode::DrawRect || mode == PaintMode::DrawHexagen || mode == PaintMode::DrawStar || mode == PaintMode::SetCenter) {
		if (this->sshape == nullptr) {
			ui.tPointText->setText("the pattern has not been created yet!");
			return;
		}
		QString sx = ui.transformTextx->toPlainText();
		QString sy = ui.transformTexty->toPlainText();
		if (sx == "" && sy == "") {
			ui.tPointText->setText("the x/y has not been set yet!");
			return;
		}
		float angle = ui.rotateText->toPlainText().toFloat();
		if (sx != "") {
			sshape->stear(angle, true);
		}
		else if (sy != "") {
			sshape->stear(angle, false);
		}
		repaint();
	}
}


void QtWidgetsApplication4::on_BazierAscend_clicked()
{
	if (mode == PaintMode::BazierCurve) {
		if (fPoints.size() >= 3) {
			fPoints = ascend(this, fPoints);
			repaint();
		}
	}
}


void QtWidgetsApplication4::on_DragPoint_clicked()
{
	if (mode == PaintMode::BazierCurve) {
		mode = PaintMode::DragPointBazier;
	}
}


void QtWidgetsApplication4::on_AddPoint_clicked()
{
	if (mode == PaintMode::DragPointBazier) {
		mode = PaintMode::BazierCurve;
	}
}

