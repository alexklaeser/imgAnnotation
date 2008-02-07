#include "AnnotationsPixmapWidget.h"
#include <QtDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QPointF>
#include <math.h>
#include <boost/foreach.hpp>
#include "MainWindow.h"
#include "functions.h"

using namespace IA;

AnnotationsPixmapWidget::AnnotationsPixmapWidget(ImgAnnotations *annotations, QAbstractScrollArea* parentScrollArea, QWidget *parent)
	: PixmapWidget(parentScrollArea, parent)
{
	mousePos = Somewhere;
	mouseMode = Normal;

	this->annotations = annotations;

	setFocusPolicy(Qt::NoFocus);
	setMouseTracking(true);
}

AnnotationsPixmapWidget::MouseMode AnnotationsPixmapWidget::getMouseMode()
{
	return mouseMode;
}

void AnnotationsPixmapWidget::onVisibleObjectsChanged(const IDList& ids)
{
	visibleObjIDs = ids;
	this->activeObjID = activeObjID;
	activeFixPoint = -1;

	repaint();
}

void AnnotationsPixmapWidget::setVisibleObjects(const IDList& ids)
{
	onVisibleObjectsChanged(ids);

	// emit the signal that the active bounding box changed
	emit activeObjectChanged(activeObjID);
}

void AnnotationsPixmapWidget::onActiveObjectChanged(ID newActiveObjID)
{
	if (newActiveObjID == activeObjID)
		return;

	activeObjID = newActiveObjID;
	activeFixPoint = -1;

	repaint();
}

void AnnotationsPixmapWidget::setActiveObject(ID newActiveObjID)
{
	if (newActiveObjID == activeObjID)
		return;

	onActiveObjectChanged(newActiveObjID);

	// emit the signal that the active bounding box changed
	emit activeObjectChanged(activeObjID);
}

void AnnotationsPixmapWidget::setMouseMode(MouseMode newMode)
{
	// don't accept the mode Dragging from outside, it's handled internally
	if (newMode == Dragging)
		return;

	// if we are dragging an object, we ignore the modes given from outside
	if (mouseMode == Dragging)
		return;

	mouseMode = newMode;

	// update the mouse cursor
	updateMouseCursor();
}


void AnnotationsPixmapWidget::paintEvent( QPaintEvent *event )
{
	// call the super class paintEvent method
	PixmapWidget::paintEvent(event);

	//
	// draw image and bounding boxes
	//

	QPainter p(this);

	// create pens and brushes
	QBrush brush(QColor(0, 0, 150, 50));
	QBrush activeBrush(QColor(150, 0, 0, 50));
	QPen pen(QColor(0, 0, 220, 200));
	QPen activePen(QColor(220, 0, 0, 200));

	// adjust the coordinate system
	p.setMatrix(getMatrix());

	// draw all inactive bounding boxes
	BOOST_FOREACH(ID objID, visibleObjIDs) {
		if (objID == activeObjID || !annotations->existsObject(objID))
			continue;

		// get the current bounding box
		Object *obj = annotations->getObject(objID);
		Q_CHECK_PTR(obj);
		QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
		QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

		// draw the rect
		p.setPen(pen);
		//p.fillRect(box, brush);
		p.drawRect(box);

		// draw the fix points (the pen has already set)
		QPointF from, to;
		for (int j = 0; j < fixPoints.count(); j++) {
			// draw a cross
			from.rx() = fixPoints[j].x() - (MARGIN + 3) / getZoomFactor();
			to.rx() = from.rx() + MARGIN / getZoomFactor();
			from.ry() = fixPoints[j].y();
			to.ry() = fixPoints[j].y();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x() + (MARGIN + 3) / getZoomFactor();
			to.rx() = from.rx() - MARGIN / getZoomFactor();
			from.ry() = fixPoints[j].y();
			to.ry() = fixPoints[j].y();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x();
			to.rx() = fixPoints[j].x();
			from.ry() = fixPoints[j].y() - (MARGIN + 3) / getZoomFactor();
			to.ry() = from.ry() + MARGIN / getZoomFactor();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x();
			to.rx() = fixPoints[j].x();
			from.ry() = fixPoints[j].y() + (MARGIN + 3) / getZoomFactor();
			to.ry() = from.ry() - MARGIN / getZoomFactor();
			p.drawLine(from, to);
		}
	}

	// draw the active bounding box
	if (annotations->existsObject(activeObjID)) {
		// get the current bounding box
		Object *obj = annotations->getObject(activeObjID);
		Q_CHECK_PTR(obj);
		QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
		QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

		// draw the rect
		p.setPen(activePen);
		p.fillRect(box, activeBrush);
		p.drawRect(box);

		// draw the fix points (the pen has already set)
		QPointF from, to;
		for (int j = 0; j < fixPoints.count(); j++) {
			// draw a cross
			from.rx() = fixPoints[j].x() - (MARGIN + 3) / getZoomFactor();
			to.rx() = from.rx() + MARGIN / getZoomFactor();
			from.ry() = fixPoints[j].y();
			to.ry() = fixPoints[j].y();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x() + (MARGIN + 3) / getZoomFactor();
			to.rx() = from.rx() - MARGIN / getZoomFactor();
			from.ry() = fixPoints[j].y();
			to.ry() = fixPoints[j].y();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x();
			to.rx() = fixPoints[j].x();
			from.ry() = fixPoints[j].y() - (MARGIN + 3) / getZoomFactor();
			to.ry() = from.ry() + MARGIN / getZoomFactor();
			p.drawLine(from, to);

			from.rx() = fixPoints[j].x();
			to.rx() = fixPoints[j].x();
			from.ry() = fixPoints[j].y() + (MARGIN + 3) / getZoomFactor();
			to.ry() = from.ry() - MARGIN / getZoomFactor();
			p.drawLine(from, to);

			// draw the number of the fix point in the upper left quadrant of the cross
			QRectF stringBox(fixPoints[j].x() - (MARGIN + 5), fixPoints[j].y() - (MARGIN + 5), (MARGIN + 5), (MARGIN + 5));
			QString str;
			str.setNum(j);
			p.drawText(stringBox, Qt::AlignHCenter | Qt::AlignVCenter, str);
		}

		// if a score value is set .. draw the score value
//		if (obj->isSet("score")) {
//			// make sure that the score is not written outside the visible area
//			QMatrix mInv = p.matrix().inverted();
//			QRectF stringBox = p.matrix().mapRect(box);
//			stringBox.setLeft(std::max(0.0, stringBox.left()));
//			stringBox.setTop(std::max(0.0, stringBox.top()));
//
//			// transform the box back in the coordinate system of the painter
//			stringBox = mInv.mapRect(stringBox);
//
//			// draw the score value on the screen
//			p.drawText(stringBox, Qt::AlignLeft | Qt::AlignTop, QString::fromStdString(obj->get("score")));
//		}
	}
}

/*void AnnotationsPixmapWidget::wheelEvent( QWheelEvent *event )
{

}*/

void AnnotationsPixmapWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (!annotations->existsObject(activeObjID))
		setCursor(Qt::ArrowCursor);

	// get the mouse coordinate in the zoomed image
	QPointF xyMouse(event->x(), event->y());
	xyMouse = getMatrixInv().map(xyMouse);
	if (mouseMode == Browsing) {
		// the mouse is selecting a bounding box .. see which box is chosen
		// the strategy: of all bounding boxes which contain the cursor take the smallest one
		mousePos = InBox;
		ID smallestBoxID = -1;
		float smallestArea = -1;
		float tmpArea;
//		for (int i = 0; i < boundingBoxes.count(); i++) {
		BOOST_FOREACH(ID objID, visibleObjIDs) {
			Object *obj = annotations->getObject(objID);
			Q_CHECK_PTR(obj);
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
			if (xyMouse.x() >= box.left() && xyMouse.x() <= box.right()
					&& xyMouse.y() >= box.top() && xyMouse.y() <= box.bottom()) {
// 				tmpDistance = sqrt(powf(box.center().x() - xyMouse.x(), 2) + powf(box.center().y() - xyMouse.y(), 2));
// 				if (closestBox < 0 || tmpDistance < closestDistance) {
// 					closestDistance = tmpDistance;
// 					closestBox = i;
// 				}
				tmpArea = box.width() * box.height();
				if (smallestBoxID < 0 || tmpArea < smallestArea) {
					smallestArea = tmpArea;
					smallestBoxID = obj->getID();
				}
			}
		}

		// set the new closest box as active box
		setActiveObject(smallestBoxID);
	}
	else if (annotations->existsObject(activeObjID)) {
		if (mouseMode == Dragging) {
			// mouse is changing the box parameters .. adapt the bounding box
			// parameters according to the mouse position
			Object *obj = annotations->getObject(activeObjID);
			Q_CHECK_PTR(obj);
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
			QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));
			if (mousePos == CloseToLeft || mousePos == CloseToTopLeft || mousePos == CloseToBottomLeft)
				// change the left
				box.setLeft(xyMouse.x());
			if (mousePos == CloseToRight || mousePos == CloseToTopRight || mousePos == CloseToBottomRight)
				// change the right
				box.setRight(xyMouse.x());
			if (mousePos == CloseToTop || mousePos == CloseToTopRight || mousePos == CloseToTopLeft)
				// change the top
				box.setTop(xyMouse.y());
			if (mousePos == CloseToBottom || mousePos == CloseToBottomRight || mousePos == CloseToBottomLeft)
				// change the bottom
				box.setBottom(xyMouse.y());
			if (mousePos == InBox) {
				// move the whole bounding box with fix points
				double newX = xyMouse.x() - clickedPosInBox.x();
				double newY = xyMouse.y() - clickedPosInBox.y();
				double dX = newX - box.left();
				double dY = newY - box.top();
				box.moveTo(newX, newY);

				// update the position of all fix points
				for (int i = 0; i < fixPoints.count(); i++) {
					fixPoints[i].rx() += dX;
					fixPoints[i].ry() += dY;
				}
			}
			if (mousePos == CloseToFixPoint && activeFixPoint >= 0 && activeFixPoint < fixPoints.count()) {
				// change the active fix point
				QPointF &point = fixPoints[activeFixPoint];
				point.setX(xyMouse.x());
				point.setY(xyMouse.y());
			}

			// save changes
			if (box.isEmpty() && 0 == box.left() && 0 == box.right() && 0 == box.top() && 0 == box.bottom())
				obj->clear("bbox");
			else
				obj->set("bbox", rect2str(box).toStdString());
			if (0 >= fixPoints.size())
				obj->clear("fixpoints");
			else
				obj->set("fixpoints", points2str(fixPoints).toStdString());

			// request a repaint
			repaint();
		}
		else if (mouseMode == CreateDeleteFixPoint) {
			Object *obj = annotations->getObject(activeObjID);
			Q_CHECK_PTR(obj);
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
			QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

			// check the distance to the fix points
			int closestPoint = -1;
			float closestDistance = -1;
			float tmpDistance;
			for (int i = 0; i < fixPoints.count(); i++) {
				tmpDistance = sqrt(powf(fixPoints[i].x() - xyMouse.x(), 2) + powf(fixPoints[i].y() - xyMouse.y(), 2));
				if (closestPoint < 0 || tmpDistance < closestDistance) {
					closestDistance = tmpDistance;
					closestPoint = i;
				}
			}
			if (closestPoint >= 0 && closestDistance <= MARGIN / getZoomFactor()) {
				mousePos = CloseToFixPoint;
				activeFixPoint = closestPoint;
			}
			else {
				mousePos = Somewhere;
				activeFixPoint = -1;
			}

			// save changes
			if (box.isEmpty() && 0 == box.left() && 0 == box.right() && 0 == box.top() && 0 == box.bottom())
				obj->clear("bbox");
			else
				obj->set("bbox", rect2str(box).toStdString());
			if (0 >= fixPoints.size())
				obj->clear("fixpoints");
			else
				obj->set("fixpoints", points2str(fixPoints).toStdString());
		}
		else if (mouseMode == Normal) {
			// mouse is not changing the box parameters .. adjust the mouse cursor

			// init variables
			bool closeToLeft = false;
			bool closeToRight = false;
			bool closeToTop = false;
			bool closeToBottom = false;
			bool inBox = false;
			bool closeToFixPoint = false;
			Object *obj = annotations->getObject(activeObjID);
			Q_CHECK_PTR(obj);
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
			QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

			// check the distance to the rect
			if (xyMouse.y() >= box.top() - MARGIN / getZoomFactor() && xyMouse.y() <= box.bottom() + MARGIN / getZoomFactor()) {
				if (fabs(xyMouse.x() - box.left()) <= MARGIN / getZoomFactor())
					closeToLeft = true;
				if (fabs(xyMouse.x() - box.right()) <= MARGIN / getZoomFactor())
					closeToRight = true;
			}
			if (xyMouse.x() >= box.left() - MARGIN / getZoomFactor() && xyMouse.x() <= box.right() + MARGIN / getZoomFactor()) {
				if (fabs(xyMouse.y() - box.top()) <= MARGIN / getZoomFactor())
					closeToTop = true;
				if (fabs(xyMouse.y() - box.bottom()) <= MARGIN / getZoomFactor())
					closeToBottom = true;
			}
			if (xyMouse.x() > box.left() && xyMouse.x() < box.right()
					&& xyMouse.y() > box.top() && xyMouse.y() < box.bottom())
				inBox = true;

			// check the distance to the fix points
			int closestPoint = -1;
			float closestDistance = -1;
			float tmpDistance;
			for (int i = 0; i < fixPoints.count(); i++) {
				tmpDistance = sqrt(powf(fixPoints[i].x() - xyMouse.x(), 2) + powf(fixPoints[i].y() - xyMouse.y(), 2));
				if (closestPoint < 0 || tmpDistance < closestDistance) {
					closestDistance = tmpDistance;
					closestPoint = i;
				}
			}
			if (closestPoint >= 0 && closestDistance <= MARGIN / getZoomFactor()) {
				closeToFixPoint = true;
				activeFixPoint = closestPoint;
			}
			else
				activeFixPoint = -1;

			// decide upon the mouse position
			if (closeToFixPoint)
				mousePos = CloseToFixPoint;
			else if (closeToLeft) {
				if (closeToTop)
					mousePos = CloseToTopLeft;
				else if (closeToBottom)
					mousePos = CloseToBottomLeft;
				else
					mousePos = CloseToLeft;
			}
			else if (closeToRight) {
				if (closeToTop)
					mousePos = CloseToTopRight;
				else if (closeToBottom)
					mousePos = CloseToBottomRight;
				else
					mousePos = CloseToRight;
			}
			else if (closeToTop)
				mousePos = CloseToTop;
			else if (closeToBottom)
				mousePos = CloseToBottom;
			else if (inBox)
				mousePos = InBox;
			else
				mousePos = Somewhere;
		}
	}

	// update the mouse cursor
	updateMouseCursor();
}

void AnnotationsPixmapWidget::mousePressEvent(QMouseEvent * event)
{
	if (!annotations->existsObject(activeObjID))
		return;

	Object *obj = annotations->getObject(activeObjID);
	Q_CHECK_PTR(obj);
	QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
	QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

	// get the mouse coordinate in the zoomed image
	QPointF xyMouse(event->x(), event->y());
	xyMouse = getMatrixInv().map(xyMouse);

	// compute the click position relative to the image
	clickedPosInBox.rx() = xyMouse.x() - box.left();
	clickedPosInBox.ry() = xyMouse.y() - box.top();

	if (mouseMode == Normal) {
		if (mousePos == Somewhere) {
			// we start a new bounding box
			box.setLeft(xyMouse.x());
			box.setTop(xyMouse.y());
			box.setRight(xyMouse.x());
			box.setBottom(xyMouse.y());

			// set the mousePos .. i.e. we are now changing the bottom right corner
			mousePos = CloseToBottomRight;
		}

		// we start dragging
		mouseMode = Dragging;

		// update the mouse cursor
		updateMouseCursor();
	}
	else if (mouseMode == CreateDeleteFixPoint) {
		// we delete/create fix points

		if (mousePos == CloseToFixPoint && activeFixPoint >= 0 && activeFixPoint < fixPoints.count()) {
			// delete the active fix point
			fixPoints.removeAt(activeFixPoint);
			activeFixPoint = -1;
			mousePos = Somewhere;
		}
		else {
			// create a new active fix point
			fixPoints.append(xyMouse);
			activeFixPoint = fixPoints.count() - 1;
			mousePos = CloseToFixPoint;
		}

		// send a signal that the bounding box has changed
		emit objectContentChanged(activeObjID);

		// update the mouse cursor
		updateMouseCursor();
	}

	// save changes
	if (box.isEmpty() && 0 == box.left() && 0 == box.right() && 0 == box.top() && 0 == box.bottom())
		obj->clear("bbox");
	else
		obj->set("bbox", rect2str(box).toStdString());
	if (0 >= fixPoints.size())
		obj->clear("fixpoints");
	else
		obj->set("fixpoints", points2str(fixPoints).toStdString());

	// request a repaint
	repaint();
}

void AnnotationsPixmapWidget::mouseReleaseEvent(QMouseEvent * event)
{
	if (mouseMode == Dragging) {
		if (annotations->existsObject(activeObjID)) {
			Object *obj = annotations->getObject(activeObjID);
			Q_CHECK_PTR(obj);
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));

			// handle the left dragged over to right size, the top over the bottom
			// side, and vice versa
			double tmp;
			if (box.left() > box.right()) {
				tmp = box.left();
				box.setLeft(box.right());
				box.setRight(tmp);
			}
			if (box.top() > box.bottom()) {
				tmp = box.top();
				box.setTop(box.bottom());
				box.setBottom(tmp);
			}

			// send the signal that the size of the bounding box has changed
			emit objectContentChanged(activeObjID);
		}

		// switch back to normal mouse mode
		mouseMode = Normal;
	}
}

void AnnotationsPixmapWidget::updateMouseCursor()
{
	// decide upon the mouse cursor for the different modes
	Qt::CursorShape newCursor = Qt::ArrowCursor;

	if (annotations->existsObject(activeObjID)) {
		if (mouseMode == Normal || mouseMode == Dragging) {
			switch (mousePos) {
				case CloseToLeft:
				case CloseToRight:
					newCursor = Qt::SizeHorCursor;
					break;
				case CloseToTop:
				case CloseToBottom:
					newCursor = Qt::SizeVerCursor;
					break;
				case CloseToTopLeft:
				case CloseToBottomRight:
					newCursor = Qt::SizeFDiagCursor;
					break;
				case CloseToTopRight:
				case CloseToBottomLeft:
					newCursor = Qt::SizeBDiagCursor;
					break;
				case CloseToFixPoint:
					newCursor = Qt::PointingHandCursor;
					break;
				case InBox:
					newCursor = Qt::SizeAllCursor;
					break;
				default:
					newCursor = Qt::CrossCursor;
					break;
			}
		}
		else if (mouseMode == CreateDeleteFixPoint) {
			switch (mousePos) {
				case CloseToFixPoint:
					newCursor = Qt::PointingHandCursor;
					break;
				default:
					newCursor = Qt::CrossCursor;
					break;
			}
		}
	}

	// set the cursor
	setCursor(newCursor);

}

/*
void AnnotationsPixmapWidget::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift) {
		browsing = true;
		event->accept();
	}
	else if (event->key() == Qt::Key_Space)
		emit requestNewObj();
	else
		event->ignore();
}

void AnnotationsPixmapWidget::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift) {
		browsing = false;
		event->accept();
	}
	else if (event->key() == Qt::Key_Space)
		event->accept();
	else
		event->ignore();
}
*/
