#include "PixmapWidget.h"
#include "MainWindow.h"

#include <QtDebug>
#include <QPixmap>
#include <QPainter>
#include <QPointF>
#include <QAbstractScrollArea>
#include <QScrollBar>


PixmapWidget::PixmapWidget(QAbstractScrollArea* parentScrollArea, QWidget *parent)
	: QWidget(parent)
{
	scrollArea = parentScrollArea;
	m_pm = new QPixmap();
	zoomFactor = 1.0;

	setMinimumSize(static_cast<int>(m_pm->width()*zoomFactor), static_cast<int>(m_pm->height()*zoomFactor));
}

PixmapWidget::~PixmapWidget()
{
	delete m_pm;
}

double PixmapWidget::getZoomFactor()
{
	return zoomFactor;
}

void PixmapWidget::setZoomFactor( double f )
{
	int w, h;

	if( fabs(f - zoomFactor) <= 0.001 )
		return;

	zoomFactor = f;
	emit( zoomFactorChanged( zoomFactor ) );

	w = static_cast<int>(m_pm->width()*zoomFactor);
	h = static_cast<int>(m_pm->height()*zoomFactor);
	setMinimumSize( w, h );

	QWidget *p = dynamic_cast<QWidget*>( parent() );
	if( p )
		resize( p->width(), p->height() );

	repaint();
}

void PixmapWidget::setPixmap(const QPixmap& pixmap)
{
	delete m_pm;
	m_pm = new QPixmap(pixmap);

	emit( pixmapChanged( m_pm ) );

	setMinimumSize(static_cast<int>(m_pm->width()*zoomFactor), static_cast<int>(m_pm->height()*zoomFactor));

	repaint();
}

void PixmapWidget::paintEvent( QPaintEvent *event )
{
	bool drawBorder = false;
	int xOffset = 0, yOffset = 0;

	if( width() > m_pm->width()*zoomFactor ) {
		xOffset = static_cast<int>((width()-m_pm->width()*zoomFactor)/2);
		drawBorder = true;
	}

	if( height() > m_pm->height()*zoomFactor ) {
		yOffset = static_cast<int>((height()-m_pm->height()*zoomFactor)/2);
		drawBorder = true;
	}

	// get the current value of the parent scroll area .. to optimize the painting
	double hValue = 0, hMin = 0, hMax = 0, hPageStep = 0, hLength = 0;
	double vValue = 0, vMin = 0, vMax = 0, vPageStep = 0, vLength = 0;
	if (scrollArea) {
		QScrollBar *scrollBar;
		scrollBar = scrollArea->horizontalScrollBar();
		if (scrollBar) {
			hValue = scrollBar->value();
			hMin = scrollBar->minimum();
			hMax = scrollBar->maximum();
			hPageStep = scrollBar->pageStep();
			hLength = hMax - hMin + hPageStep;
		}
		scrollBar = scrollArea->verticalScrollBar();
		if (scrollBar) {
			vValue = scrollBar->value();
			vMin = scrollBar->minimum();
			vMax = scrollBar->maximum();
			vPageStep = scrollBar->pageStep();
			vLength = vMax - vMin + vPageStep;
		}
	}

	//
	// draw image and bounding boxes
	//

	QPainter p(this);

	// adjust the coordinate system
	p.save();
	p.translate(xOffset, yOffset);
	p.scale(zoomFactor, zoomFactor);
	currentMatrix = p.matrix();
	currentMatrixInv = currentMatrix.inverted();

	// find out which part of the image we have to draw
	// i.e. if we are embedded into a QScrollArea and not all is visible
	QPointF startPoint((hValue / hLength) * m_pm->width(), (vValue / vLength) * m_pm->height());
	QRectF imgVisible;
	imgVisible.setLeft(startPoint.x());
	imgVisible.setWidth((hPageStep / hLength) * m_pm->width());
	imgVisible.setTop(startPoint.y());
	imgVisible.setHeight((vPageStep / vLength) * m_pm->height());

	// draw the image
	p.drawPixmap(startPoint, *m_pm, imgVisible);

	// draw a border around the image
	p.restore();
	if (drawBorder) {
		p.setPen( Qt::black );
		p.drawRect(xOffset-1, yOffset-1, static_cast<int>(m_pm->width()*zoomFactor+1), static_cast<int>(m_pm->height()*zoomFactor+1));
	}
}

QMatrix PixmapWidget::getMatrixInv() const
{
	return currentMatrixInv;
}

QMatrix PixmapWidget::getMatrix() const
{
	return currentMatrix;
}


/*void PixmapWidget::wheelEvent( QWheelEvent *event )
{

}*/


/*
void PixmapWidget::keyPressEvent(QKeyEvent * event)
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

void PixmapWidget::keyReleaseEvent(QKeyEvent * event)
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
