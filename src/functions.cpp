#include "functions.h"
#include <boost/foreach.hpp>
#include <QListIterator>


QStringList std2qt(const std::list<std::string> &stdList) {
	QStringList qList;
	BOOST_FOREACH(std::string str, stdList)
		qList << QString::fromStdString(str);
	return qList;
}

std::list<std::string> qt2std(const QStringList &qList) {
	std::list<std::string> stdList;
	foreach(QString str, qList)
		stdList.push_back(str.toStdString());
	return stdList;
}

QRectF str2rect(const QString &str) {
	QRectF rect;
	QStringList parts = str.split(",");
	if (parts.count() != 4)
		return rect;

	QList<double> doubles;
	foreach (QString part, parts)
		doubles << part.trimmed().toDouble();

	if (doubles.count() != 4)
		return rect;

	rect.setLeft(doubles[0]);
	rect.setTop(doubles[1]);
	rect.setWidth(doubles[2]);
	rect.setHeight(doubles[3]);

	return rect;
}

QString rect2str(const QRectF &rect) {
	QString str;
	str = QString::number(rect.left()) + ","
			+ QString::number(rect.top()) + ","
			+ QString::number(rect.width()) + ","
			+ QString::number(rect.height());
	return str;
}

QList<QPointF> str2points(const QString &str) {
	QList<QPointF> points;
	QStringList parts = str.split(",");
	if (0 != parts.count() % 2)
		return points;

	QList<double> doubles;
	foreach (QString part, parts)
		doubles << part.trimmed().toDouble();

	if (0 != doubles.count() % 2)
		return points;

	for (int i = 0; i < doubles.count(); i += 2) {
		QPointF point;
		point.setX(doubles[i]);
		point.setY(doubles[i + 1]);
		points << point;
	}

	return points;
}

QString points2str(const QList<QPointF> &points) {
	QString str;
	QListIterator<QPointF> i(points);
	while (i.hasNext()) {
		QPointF point = i.next();
		str += QString::number(point.x()) + "," + QString::number(point.y());
		if (i.hasNext())
			str += ",";
	}
	return str;
}
