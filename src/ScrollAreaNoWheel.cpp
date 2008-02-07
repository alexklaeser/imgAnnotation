#include "ScrollAreaNoWheel.h"

ScrollAreaNoWheel::ScrollAreaNoWheel(QWidget *parent) : QScrollArea( parent )
{
	setFocusPolicy(Qt::NoFocus);
}

void ScrollAreaNoWheel::wheelEvent(QWheelEvent *event)
{
	// send event further
	event->ignore();
	emit wheelTurned(event);
	
// QWidget *parent = parentWidget();
// 	if (parent != NULL)
// 		parent->wheelEvent(event);
}
