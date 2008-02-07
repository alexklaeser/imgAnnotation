#ifndef PIXMAPWIDGET_H
#define PIXMAPWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QMatrix>

#define MARGIN 5


class QAbstractScrollArea;


// our own pixmap widget .. which displays an image and bounding boxes
class PixmapWidget : public QWidget
{
	Q_OBJECT

private:
	QPixmap *m_pm;
	double zoomFactor;
	QMatrix currentMatrixInv;
	QMatrix currentMatrix;
	QAbstractScrollArea *scrollArea;

protected:
	void paintEvent(QPaintEvent*);
//	void wheelEvent(QWheelEvent*);
	QMatrix getMatrixInv() const;
	QMatrix getMatrix() const;

public:
	PixmapWidget( QAbstractScrollArea* scrollArea, QWidget *parent=0 );
	~PixmapWidget();
	double getZoomFactor();

public slots:
	void setZoomFactor(double);
	void setPixmap(const QPixmap&);

signals:
	void zoomFactorChanged(double);
	void pixmapChanged(QPixmap*);

};

#endif // PIXMAPWIDGET_H
