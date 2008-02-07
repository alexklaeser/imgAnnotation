#ifndef ANNOTATIONPIXMAPWIDGET_H_
#define ANNOTATIONPIXMAPWIDGET_H_

#include "PixmapWidget.h"

#include <ImgAnnotations.h>

#define MARGIN 5


class QMouseEvent;


//// defines a bounding box that is to be drawn on the screen
//class BoundingBox
//{
//public:
//	QRectF box;
//	QList<QPointF> fixPoints;
//	double score;
//};


// our own pixmap widget .. which displays an image and bounding boxes
class AnnotationsPixmapWidget : public PixmapWidget
{
	Q_OBJECT

public:
	enum MouseMode {
		// normal mouse mode .. resizing and moving the bounding boxes
		Normal,
		// the mouse is exploring (browsing) the bounding boxes, i.e. selcting
		// a new box visually
		Browsing,
		// the user creates by clicking a new fix point .. clicking on the on a
		// point already existing will delete the point
		CreateDeleteFixPoint,
		// the mouse is dragging/resizing an object .. cannot be set via
		// setMouseMode()
		Dragging
	};

private:
	enum MousePosition {
		Somewhere, InBox, CloseToFixPoint,
		CloseToLeft, CloseToRight, CloseToTop, CloseToBottom,
		CloseToTopLeft, CloseToTopRight, CloseToBottomLeft, CloseToBottomRight
	};

	IA::ImgAnnotations *annotations;
	IA::IDList visibleObjIDs;
	IA::ID activeObjID;
	int activeFixPoint;

	MousePosition mousePos;
	MouseMode mouseMode;
	QPointF clickedPosInBox;
	QAbstractScrollArea *scrollArea;

public:
	AnnotationsPixmapWidget( IA::ImgAnnotations *annotations, QAbstractScrollArea* scrollArea, QWidget *parent=0 );
//	~AnnotationsPixmapWidget();
	MouseMode getMouseMode();
	void setMouseMode(MouseMode);
	void setVisibleObjects(const IA::IDList&);
	void setActiveObject(IA::ID);

public slots:
	void onVisibleObjectsChanged(const IA::IDList&);
	void onActiveObjectChanged(IA::ID);

signals:
	void objectContentChanged(IA::ID);
	void activeObjectChanged(IA::ID);
//	void requestNewObj();

protected:
	void paintEvent(QPaintEvent*);
//	void wheelEvent(QWheelEvent*);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
//	void keyPressEvent(QKeyEvent * event);
//	void keyReleaseEvent(QKeyEvent * event);

private:
	void updateMouseCursor();
};

#endif /*ANNOTATIONPIXMAPWIDGET_H_*/
