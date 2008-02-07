#ifndef ScrollAreaNoWheel_H
#define ScrollAreaNoWheel_H

#include <QScrollArea>
#include <QWheelEvent>

// defines our own ScrollArea which behaves a bit more special
// in the way that it will ignore QWheelEvents
class ScrollAreaNoWheel : public QScrollArea
{
	Q_OBJECT

public:
	ScrollAreaNoWheel(QWidget *parent=0);
	
protected:
	void wheelEvent(QWheelEvent *);

signals:
	void wheelTurned(QWheelEvent *);
};


#endif
