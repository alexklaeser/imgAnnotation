#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <QString>
#include <QPointF>
#include <QList>
#include <QStringList>
#include <list>
#include <string>
#include <QRectF>


#ifndef MIN
	#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
	#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

QStringList std2qt(const std::list<std::string> &stdList);
std::list<std::string> qt2std(const QStringList &qList);
QRectF str2rect(const QString &str);
QString rect2str(const QRectF &rect);
QList<QPointF> str2points(const QString &str);
QString points2str(const QList<QPointF> &points);

#endif /*FUNCTIONS_H_*/
