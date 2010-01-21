#include "ImageExtractDialog.h"
#include "PixmapWidget.h"
#include <QHash>
#include <QList>
#include <QTreeWidget>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include <QTextStream>
#include <QHeaderView>
#include <QTableWidget>
#include <QFile>
#include <QDir>
#include <QtGlobal>
#include <QtDebug>

#include <math.h>
#include <string>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#ifndef NO_OPENCV
#include <opencv/cv.h>
#include <opencv/functions.h>
#include <opencv/IplImageWrapper.h>
#endif

#include <ImgAnnotations.h>
#include "functions.h"

using namespace IA;
using boost::algorithm::iequals;
using std::string;


/**
 * helper function
 */
ImageExtractDialog::StrStrMultiMap getPropertiesFromTableWidget(QTableWidget* tableWidget)
{
	ImageExtractDialog::StrStrMultiMap properties;
	for (int iRow = 0; iRow < tableWidget->rowCount(); ++iRow) {
		// read the property/value from the table row
		QTableWidgetItem *i;
		i = tableWidget->item(iRow, 0);
		if (NULL == i)
			continue;
		string property = i->text().toStdString();
		i = tableWidget->item(iRow, 1);
		string value;
		if (NULL == i)
			value = "";
		else
			value = i->text().toStdString();

		// check wether the property is empty
		if (property.empty())
			continue;

		properties.insert(IA::StrStrPair(property, value));
	}
	return properties;
}


ImageExtractDialog::ImageExtractDialog(ImgAnnotations *annotations, QString *databasePath, QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	orgDatabasePath = databasePath;
	orgAnnotations = annotations;
	normMinTop = 0;
	normMaxBottom = 0;
	normMinLeft = 0;
	normMaxRight = 0;
	meanRotation = 0;
	selectedCenter = BoundingBoxCenter;
	selectedScale = HeightScale;
	selectedFlipHorizontally = false;
	selectedFlipVertically = false;
	normalizeRotation = false;

	// init the random generator
	random.seed(static_cast<unsigned> (std::time(0)));

	// set up the GUI
	setupUi(this);
	centerComboBox->addItem("bounding box");
	centerComboBox->addItem("fix points");
	centerComboBox->setCurrentIndex(0);
	scaleNormComboBox->addItem("longest side");
	scaleNormComboBox->addItem("height");
	scaleNormComboBox->addItem("width");
	scaleNormComboBox->addItem("std. dev. of fix points");
	scaleNormComboBox->setCurrentIndex(0);
	minDimensionComboBox->addItem("longest side");
	maxDimensionComboBox->addItem("shortest side");
	flipDirectionComboBox->addItem("horizontally");
	flipDirectionComboBox->addItem("vertically");
	flipDirectionComboBox->addItem("rotate 180 deg.");
	flipDirectionComboBox->setCurrentIndex(0);
	rotationNormComboBox1->addItem("<no alignment>");
	rotationNormComboBox1->addItem("center fix points");
	rotationNormComboBox1->addItem("fix point 0");
	rotationNormComboBox1->addItem("fix point 1");
	rotationNormComboBox1->addItem("fix point 2");
	rotationNormComboBox1->addItem("fix point 3");
	rotationNormComboBox1->addItem("fix point 4");
	rotationNormComboBox1->addItem("fix point 5");
	rotationNormComboBox1->setCurrentIndex(0);
	rotationNormComboBox2->addItem("<no alignment>");
	rotationNormComboBox2->addItem("center fix points");
	rotationNormComboBox2->addItem("fix point 0");
	rotationNormComboBox2->addItem("fix point 1");
	rotationNormComboBox2->addItem("fix point 2");
	rotationNormComboBox2->addItem("fix point 3");
	rotationNormComboBox2->addItem("fix point 4");
	rotationNormComboBox2->addItem("fix point 5");
	rotationNormComboBox2->setCurrentIndex(0);
	propertiesTableWidget->horizontalHeader()->setStretchLastSection(true);
	notPropertiesTableWidget->horizontalHeader()->setStretchLastSection(true);
	propertiesTableWidget2->horizontalHeader()->setStretchLastSection(true);
	notPropertiesTableWidget2->horizontalHeader()->setStretchLastSection(true);
//	propertiesTableWidget->horizontalHeaderItem(1)->setResizeMode(QHeaderView::Stretch);
//	notPropertiesTableWidget->horizontalHeaderItem(1)->setResizeMode(QHeaderView::Stretch);

	// add the PixmapWidget to the scrollArea
	pixmapWidget = new PixmapWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(pixmapWidget);

	// make some connections
	connect(scrollArea, SIGNAL(wheelTurned(QWheelEvent*)), this, SLOT(onWheelTurnedInScrollArea(QWheelEvent *)));
	connect(leftBorderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateOutputSizeLabel()));
	connect(rightBorderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateOutputSizeLabel()));
	connect(topBorderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateOutputSizeLabel()));
	connect(bottomBorderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateOutputSizeLabel()));
}

void ImageExtractDialog::fillAnnotationData()
{
//	// add all the tags we have to the tagListWidget and notTagListWidget
//	QStringList tmpList = orgAnnotations->gffetAllTags();
//	tmpList.sort();qwedq wed
//
//	// decide wether to update the view or not
//	bool doUpdate = false;
//	if (tagListWidget->count() != tmpList.count())
//		doUpdate = true;
//	for (int i = 0; i < tmpList.count() && !doUpdate; i++) {
//		QList<QListWidgetItem *> items = tagListWidget->findItems(tmpList[i], Qt::MatchExactly);
//		if (items.count() == 0)
//			doUpdate = true;
//	}
//	if (doUpdate) {
//		tagListWidget->clear();
//		tagListWidget->addItems(tmpList);
//		notTagListWidget->clear();
//		notTagListWidget->addItems(tmpList);
//		flipTagComboBox->clear();
//		flipTagComboBox->addItems(tmpList);
//		flipTagComboBox->setCurrentIndex(0);
//	}
//
//	// add all the object types to the objTypeComoboBox
//	tmpList = orgAnnotations->getAllObjTypes();
//	tmpList.sort();
//
//	// decide wether to update the view or not
//	doUpdate = false;
//	if (objTypeComboBox->count() != tmpList.count())
//		doUpdate = true;
//	for (int i = 0; i < tmpList.count() && !doUpdate; i++) {
//		if (objTypeComboBox->findText(tmpList[i], Qt::MatchExactly) < 0)
//			doUpdate = true;
//	}
//	if (doUpdate) {
//		objTypeComboBox->clear();
//		objTypeComboBox->addItems(tmpList);
//	}
}

void ImageExtractDialog::filterData()
{

	// clear the previously filtered data
	filteredObjIDs.clear();

	//
	// prepare data for the filtration process
	//

	// get the properties that have to and that must not occur
	selectedProperties = getPropertiesFromTableWidget(propertiesTableWidget);
	selectedNotProperties = getPropertiesFromTableWidget(notPropertiesTableWidget);

	// check how many fix points the object should have
	selectedNumOfFixPoints = -1;
	if (Qt::Checked == numOfFixPointsCheckBox->checkState())
		selectedNumOfFixPoints = numOfFixPointsSpinBox->value();

	// ensure that we have annotation data
	if (NULL == orgAnnotations)
		return;

	//
	// filter the original data
	//

	BOOST_FOREACH(Object *obj, orgAnnotations->getObjects()) {
		// check wether one of the not-properties matches
		bool hasNotTag = false;
		BOOST_FOREACH(StrStrPair entry, selectedNotProperties) {
			Q_ASSERT(!entry.first.empty());
			// note if the value is empty, it matches anything
			if (obj->isSet(entry.first) &&
				(entry.second.empty() || obj->get(entry.first) == entry.second))
				hasNotTag = true;
		}
		if (hasNotTag)
			continue;

		// check wether all selected tags are given
		bool allTagsThere = true;
		BOOST_FOREACH(StrStrPair entry, selectedProperties) {
			Q_ASSERT(!entry.first.empty());
			// note if the value is empty, it matches anything
			if (!(obj->isSet(entry.first) &&
				(entry.second.empty() || iequals(obj->get(entry.first), entry.second))))
				allTagsThere = false;
			if (!allTagsThere)
				break;
		}
		if (!allTagsThere)
			continue;

		// check for the right number of fixPoints
		if (selectedNumOfFixPoints >= 0) {
			QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));
			if (fixPoints.count() != selectedNumOfFixPoints)
				continue;
		}

		// if we come to here it means that we found an object fullfulling
		// the user's criteria .. we add it to the filtered data
		filteredObjIDs.push_back(obj->getID());
	}

	//
	// get the objects that are filtered for the subset of extracted objects
	//

	// clear the previously filtered data
	filteredObjIDs2.clear();

	// get the properties that have to and that must not occur
	selectedProperties2 = getPropertiesFromTableWidget(propertiesTableWidget2);
	selectedNotProperties2 = getPropertiesFromTableWidget(notPropertiesTableWidget2);

	// get the min and max dimensions
	selectedMinDimension = -1;
	selectedMaxDimension = -1;
	if (Qt::Checked == minDimensionCheckBox->checkState())
		selectedMinDimension = minDimensionSpinBox->value();
	if (Qt::Checked == maxDimensionCheckBox->checkState())
		selectedMaxDimension = maxDimensionSpinBox->value();

	// ensure that we have annotation data
	if (NULL == orgAnnotations)
		return;

	//
	// filter the set from the previous filtering
	//

	BOOST_FOREACH(IA::ID objID, filteredObjIDs) {
		// get the object by its ID
		Object *obj = orgAnnotations->getObject(objID);

		// check wether one of the not-properties matches
		bool hasNotTag = false;
		BOOST_FOREACH(StrStrPair entry, selectedNotProperties2) {
			Q_ASSERT(!entry.first.empty());
			// note if the value is empty, it matches anything
			if (obj->isSet(entry.first) &&
				(entry.second.empty() || obj->get(entry.first) == entry.second))
				hasNotTag = true;
		}
		if (hasNotTag)
			continue;

		// check wether all selected tags are given
		bool allTagsThere = true;
		BOOST_FOREACH(StrStrPair entry, selectedProperties2) {
			Q_ASSERT(!entry.first.empty());
			// note if the value is empty, it matches anything
			if (!(obj->isSet(entry.first) &&
				(entry.second.empty() || iequals(obj->get(entry.first), entry.second))))
				allTagsThere = false;
			if (!allTagsThere)
				break;
		}
		if (!allTagsThere)
			continue;

		// check for the right dimensions
		if (selectedMinDimension >= 0 || selectedMaxDimension >= 0) {
			QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
			if (selectedMinDimension >= 0 && MIN(box.width(), box.height()) < selectedMinDimension)
				continue;
			if (selectedMaxDimension >= 0 && MAX(box.width(), box.height()) > selectedMaxDimension)
				continue;
		}

		// if we come to here it means that we found an object fullfulling
		// the user's criteria .. we add it to the filtered data
		filteredObjIDs2.push_back(obj->getID());
	}
}

void ImageExtractDialog::computeAlignment()
{
	// check how many fix points the object should have
	int selectedNumOfFixPoints = -1;
	if (Qt::Checked == numOfFixPointsCheckBox->checkState())
		selectedNumOfFixPoints = numOfFixPointsSpinBox->value();

	// check wether everything is set right
	if (selectedCenter == FixPointsCenter && selectedNumOfFixPoints < 1)
		return;
	if (selectedScale == StdDevFixPointsScale && selectedNumOfFixPoints < 2)
		return;

	//
	// compute the alignment for all filtered objects
	//

	bool justStarted = true;

	// loop through the filtered objects and add them to the filteredObjTreeWidget
	QPointF centerFixPoints, centerPoint;
	double norm;
	meanRotation = 0;
	BOOST_FOREACH(ID objID, filteredObjIDs) {
		Object *obj = orgAnnotations->getObject(objID);
		Q_CHECK_PTR(obj);
		QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
		QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

		// ensure that we have enough fix points and a bounding box
		if (selectedCenter == FixPointsCenter && fixPoints.count() < 1)
			continue;
		if (selectedScale == StdDevFixPointsScale && fixPoints.count() < 2)
			continue;
		if (box.isEmpty())
			continue;

		// compute the selectedCenter point of the fix points
		centerFixPoints.rx() = 0;
		centerFixPoints.ry() = 0;
		if (fixPoints.count() > 0) {
			for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++)
				centerFixPoints += fixPoints[iFixPoint];
			centerFixPoints.rx() /= fixPoints.count();
			centerFixPoints.ry() /= fixPoints.count();
		}

		// compute the normalization factor
		if (selectedScale == StdDevFixPointsScale) {
			// compute the std deviation as normalization for x and y direction
			norm = 0;
			if (fixPoints.count() > 0) {
				for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++) {
					norm += pow(centerFixPoints.x() - fixPoints[iFixPoint].x(), 2);
					norm += pow(centerFixPoints.y() - fixPoints[iFixPoint].y(), 2);
				}
				norm = sqrt(norm / (2 * fixPoints.count()));
			}
		}
		else if (selectedScale == HeightScale)
			norm = box.height();
		else if (selectedScale == WidthScale)
			norm = box.width();
		else
			// otherwise we take the longest side of the bounding box as factor
			norm = MAX(box.width(), box.height());

		// set the selectedCenter point of the object
		if (selectedCenter == FixPointsCenter)
			centerPoint = centerFixPoints;
		else
			centerPoint = box.center();

		// compute the rotation of the sample with help of the fix points
		double rotation = 0;
		if (normalizeRotation) {
			int i1 = rotationNormComboBox1->currentIndex();
			int i2 = rotationNormComboBox2->currentIndex();
			Q_ASSERT(i1 > 0);
			Q_ASSERT(i2 > 0);
			Q_ASSERT(i1 != i2);

			// get the correct points in order to compute the rotation
			QPointF rotationPoint1, rotationPoint2;
			if (i1 == 1)
				rotationPoint1 = centerFixPoints;
			else {
				Q_ASSERT(i1 - 2 >= 0);
				Q_ASSERT(i1 - 2 < fixPoints.count());
				rotationPoint1 = fixPoints[i1 - 2];
			}
			if (i2 == 1)
				rotationPoint2 = centerFixPoints;
			else {
				Q_ASSERT(i2 - 2 >= 0);
				Q_ASSERT(i2 - 2 < fixPoints.count());
				rotationPoint2 = fixPoints[i2 - 2];
			}

			// compute the orientation
			rotation = atan2(rotationPoint1.x() - rotationPoint2.x(), rotationPoint1.y() - rotationPoint2.y());
			while (rotation < 0)
				rotation += 2 * M_PI;
			while (rotation > 2 * M_PI)
				rotation -= 2 * M_PI;
		}

		// duplicate the bounding box and flip its dimensions if necessary
		// also take into account the rotation
		QRectF oldBox = box;
		if (!selectedFlipProperty.isEmpty() && obj->isSet(selectedFlipProperty.toStdString()) &&
			(selectedFlipValue.isEmpty() || iequals(obj->get(selectedFlipProperty.toStdString()), selectedFlipValue.toStdString()))) {
			if (selectedFlipHorizontally) {
				box.setLeft(2 * centerPoint.x() - oldBox.right());
				box.setRight(2 * centerPoint.x() - oldBox.left());
//				rotation *= -1;
			}
			if (selectedFlipVertically) {
				box.setTop(2 * centerPoint.y() - oldBox.bottom());
				box.setBottom(2 * centerPoint.y() - oldBox.top());
//				rotation = M_PI - rotation;
			}
			while (rotation < 0)
				rotation += 2 * M_PI;
			while (rotation > 2 * M_PI)
				rotation -= 2 * M_PI;
		}

		// update the mean rotation
		meanRotation += rotation;

		// update the min/max dimensions
		if (justStarted) {
			normMinTop = (box.top() - centerPoint.y()) / norm;
			normMaxBottom = (box.bottom() - centerPoint.y()) / norm;
			normMinLeft = (box.left() - centerPoint.x()) / norm;
			normMaxRight = (box.right() - centerPoint.x()) / norm;
			justStarted = false;
		}
		else {
			normMinTop = MIN(normMinTop, (box.top() - centerPoint.y()) / norm);
			normMaxBottom = MAX(normMaxBottom, (box.bottom() - centerPoint.y()) / norm);
			normMinLeft = MIN(normMinLeft, (box.left() - centerPoint.x()) / norm);
			normMaxRight = MAX(normMaxRight, (box.right() - centerPoint.x()) / norm);
		}
	}
	meanRotation /= filteredObjIDs.size();

	// check for horizontal/vertical symmetry
	if (hSymmetryCheckBox->isChecked()) {
		double maxSide = MAX(normMaxRight, -normMinLeft);
		normMinLeft = -maxSide;
		normMaxRight = maxSide;
	}
	if (vSymmetryCheckBox->isChecked()) {
		double maxSide = MAX(normMaxBottom, -normMinTop);
		normMinTop = -maxSide;
		normMaxBottom = maxSide;
	}
}

QImage ImageExtractDialog::loadImg(QString filePath)
{
	// check wether we have a relative or absolute path
	if (!QDir::isAbsolutePath(filePath)) filePath = *orgDatabasePath + "/" + filePath;

	//printf("file: %s\n", filePath.toAscii().constData());
	return QImage(filePath);
}

QRectF ImageExtractDialog::getCropRect(ID objID)
{
	// check wether dir/file/object have been selected
	Object *obj = orgAnnotations->getObject(objID);
	if (NULL == obj)
		return QRectF();

	//
	// crop the object out of the image
	//

	QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
	QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

	// compute the selectedCenter point of the fix points
	double norm;
	QPointF centerFixPoints, centerPoint;
	centerFixPoints.rx() = 0;
	centerFixPoints.ry() = 0;
	if (fixPoints.count() > 0) {
		for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++)
			centerFixPoints += fixPoints[iFixPoint];
		centerFixPoints.rx() /= fixPoints.count();
		centerFixPoints.ry() /= fixPoints.count();
	}

	// compute the normalization factor
	if (selectedScale == StdDevFixPointsScale) {
		// compute the std deviation as normalization for x and y direction
		norm = 0;
		if (fixPoints.count() > 0) {
			for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++) {
				norm += pow(centerFixPoints.x() - fixPoints[iFixPoint].x(), 2);
				norm += pow(centerFixPoints.y() - fixPoints[iFixPoint].y(), 2);
			}
			norm = sqrt(norm / (2 * fixPoints.count()));
		}
	}
	else if (selectedScale == HeightScale)
		norm = box.height();
	else if (selectedScale == WidthScale)
		norm = box.width();
	else
		// otherwise we take the longest side of the bounding box as factor
		norm = MAX(box.width(), box.height());

	// set the selectedCenter point of the object
	if (selectedCenter == FixPointsCenter)
		centerPoint = centerFixPoints;
	else
		centerPoint = box.center();

	// compute the rotation of the sample with help of the fix points
	double rotation = 0;
	if (normalizeRotation) {
		int i1 = rotationNormComboBox1->currentIndex();
		int i2 = rotationNormComboBox2->currentIndex();
		Q_ASSERT(i1 > 0);
		Q_ASSERT(i2 > 0);
		Q_ASSERT(i1 != i2);

		// get the correct points in order to compute the rotation
		QPointF rotationPoint1, rotationPoint2;
		if (i1 == 1)
			rotationPoint1 = centerFixPoints;
		else {
			Q_ASSERT(i1 - 2 >= 0);
			Q_ASSERT(i1 - 2 < fixPoints.count());
			rotationPoint1 = fixPoints[i1 - 2];
		}
		if (i2 == 1)
			rotationPoint2 = centerFixPoints;
		else {
			Q_ASSERT(i2 - 2 >= 0);
			Q_ASSERT(i2 - 2 < fixPoints.count());
			rotationPoint2 = fixPoints[i2 - 2];
		}
		// compute the orientation relative to the mean rotation
		rotation = atan2(rotationPoint1.x() - rotationPoint2.x(), rotationPoint1.y() - rotationPoint2.y());
		while (rotation < 0)
			rotation += 2 * M_PI;
		while (rotation > 2 * M_PI)
			rotation -= 2 * M_PI;
	}

	// compute which part of the image we have to crop
	QRectF cropF;
	cropF.setTop(centerPoint.y() + normMinTop * norm);
	cropF.setBottom(centerPoint.y() + normMaxBottom * norm);
	cropF.setLeft(centerPoint.x() + normMinLeft * norm);
	cropF.setRight(centerPoint.x() + normMaxRight * norm);

	// compute the size we are going to.scale the cropped image to
	double width = widthSpinBox->value();
	double height = heightSpinBox->value();

	// compute where the selectedCenter point lies in the image
	double centerImgX = (width * -1 * normMinLeft) / (normMaxRight - normMinLeft);
	double centerImgY = (height * -1 * normMinTop) / (normMaxBottom - normMinTop);

	// get the zoom factor .. i.e. the smallest factor given by the distance of
	// the selectedCenter point in the output image and its borders divided by the
	// object selectedCenter point in the original image and its bounding box borders
	double zoomFactor = 1000000; // some big number to init the value
	if (centerPoint.x() - cropF.left() != 0)
		zoomFactor = MIN(zoomFactor, centerImgX / (centerPoint.x() - cropF.left()));
	if (cropF.right() - centerPoint.x() != 0)
		zoomFactor = MIN(zoomFactor, (width - centerImgX) / (cropF.right() - centerPoint.x()));
	if (centerPoint.y() - cropF.top() != 0)
		zoomFactor = MIN(zoomFactor, centerImgY / (centerPoint.y() - cropF.top()));
	if (cropF.bottom() - centerPoint.x() != 0)
		zoomFactor = MIN(zoomFactor, (height - centerImgY) / (cropF.bottom() - centerPoint.y()));

	// get the margins
	int borderTop = topBorderSpinBox->value();
	int borderBottom = bottomBorderSpinBox->value();
	int borderLeft = leftBorderSpinBox->value();
	int borderRight = rightBorderSpinBox->value();
	
	// add margins
	cropF.setLeft(cropF.left() - borderLeft / zoomFactor);
	cropF.setRight(cropF.right() + borderRight / zoomFactor);
	cropF.setTop(cropF.top() - borderTop / zoomFactor);
	cropF.setBottom(cropF.bottom() + borderBottom / zoomFactor);
	
	return cropF;
}


QPixmap ImageExtractDialog::cropObj(QImage &imgFile, ID objID, bool drawBoundingBox,
		double xTranslate, double yTranslate, double scale,
		double aspectRatio, double xShear, double yShear, double rotateDegree, double imgSizeScale,
		double smoothing, double contrastReduction, double noise)

{
#ifndef NO_OPENCV
	// check wether dir/file/object have been selected
	Object *obj = orgAnnotations->getObject(objID);
	if (NULL == obj)
		return QPixmap();

	//
	// crop the object out of the image
	//

	QRectF box = str2rect(QString::fromStdString(obj->get("bbox")));
	QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

	// compute the selectedCenter point of the fix points
	double norm;
	QPointF centerFixPoints, centerPoint;
	centerFixPoints.rx() = 0;
	centerFixPoints.ry() = 0;
	if (fixPoints.count() > 0) {
		for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++)
			centerFixPoints += fixPoints[iFixPoint];
		centerFixPoints.rx() /= fixPoints.count();
		centerFixPoints.ry() /= fixPoints.count();
	}

	// compute the normalization factor
	if (selectedScale == StdDevFixPointsScale) {
		// compute the std deviation as normalization for x and y direction
		norm = 0;
		if (fixPoints.count() > 0) {
			for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++) {
				norm += pow(centerFixPoints.x() - fixPoints[iFixPoint].x(), 2);
				norm += pow(centerFixPoints.y() - fixPoints[iFixPoint].y(), 2);
			}
			norm = sqrt(norm / (2 * fixPoints.count()));
		}
	}
	else if (selectedScale == HeightScale)
		norm = box.height();
	else if (selectedScale == WidthScale)
		norm = box.width();
	else
		// otherwise we take the longest side of the bounding box as factor
		norm = MAX(box.width(), box.height());

	// set the selectedCenter point of the object
	if (selectedCenter == FixPointsCenter)
		centerPoint = centerFixPoints;
	else
		centerPoint = box.center();

	// compute the rotation of the sample with help of the fix points
	double rotation = 0;
	if (normalizeRotation) {
		int i1 = rotationNormComboBox1->currentIndex();
		int i2 = rotationNormComboBox2->currentIndex();
		Q_ASSERT(i1 > 0);
		Q_ASSERT(i2 > 0);
		Q_ASSERT(i1 != i2);

		// get the correct points in order to compute the rotation
		QPointF rotationPoint1, rotationPoint2;
		if (i1 == 1)
			rotationPoint1 = centerFixPoints;
		else {
			Q_ASSERT(i1 - 2 >= 0);
			Q_ASSERT(i1 - 2 < fixPoints.count());
			rotationPoint1 = fixPoints[i1 - 2];
		}
		if (i2 == 1)
			rotationPoint2 = centerFixPoints;
		else {
			Q_ASSERT(i2 - 2 >= 0);
			Q_ASSERT(i2 - 2 < fixPoints.count());
			rotationPoint2 = fixPoints[i2 - 2];
		}
		// compute the orientation relative to the mean rotation
		rotation = atan2(rotationPoint1.x() - rotationPoint2.x(), rotationPoint1.y() - rotationPoint2.y());
		while (rotation < 0)
			rotation += 2 * M_PI;
		while (rotation > 2 * M_PI)
			rotation -= 2 * M_PI;
	}

	// compute which part of the image we have to crop
	QRectF cropF;
	cropF.setTop(centerPoint.y() + normMinTop * norm);
	cropF.setBottom(centerPoint.y() + normMaxBottom * norm);
	cropF.setLeft(centerPoint.x() + normMinLeft * norm);
	cropF.setRight(centerPoint.x() + normMaxRight * norm);

	// compute the size we are going to.scale the cropped image to
	double width = widthSpinBox->value() * imgSizeScale;
	double height = heightSpinBox->value() * imgSizeScale;

	// compute where the selectedCenter point lies in the image
	double centerImgX = (width * -1 * normMinLeft) / (normMaxRight - normMinLeft);
	double centerImgY = (height * -1 * normMinTop) / (normMaxBottom - normMinTop);

	// get the zoom factor .. i.e. the smallest factor given by the distance of
	// the selectedCenter point in the output image and its borders divided by the
	// object selectedCenter point in the original image and its bounding box borders
	double zoomFactor = 1000000; // some big number to init the value
	if (centerPoint.x() - cropF.left() != 0)
		zoomFactor = MIN(zoomFactor, centerImgX / (centerPoint.x() - cropF.left()));
	if (cropF.right() - centerPoint.x() != 0)
		zoomFactor = MIN(zoomFactor, (width - centerImgX) / (cropF.right() - centerPoint.x()));
	if (centerPoint.y() - cropF.top() != 0)
		zoomFactor = MIN(zoomFactor, centerImgY / (centerPoint.y() - cropF.top()));
	if (cropF.bottom() - centerPoint.x() != 0)
		zoomFactor = MIN(zoomFactor, (height - centerImgY) / (cropF.bottom() - centerPoint.y()));

	// get the margins
	int borderTop = topBorderSpinBox->value();
	int borderBottom = bottomBorderSpinBox->value();
	int borderLeft = leftBorderSpinBox->value();
	int borderRight = rightBorderSpinBox->value();

	// compute a matrix for the transformation of the original image coordinate
	// system to the coordinate system of the patch we would like to extract ...
	// this is easier for the thinking .. afterwards we will take the inverse
	QMatrix transform;
	transform.translate(cropF.left(), cropF.top());
	transform.scale(1 / zoomFactor, 1 / zoomFactor);
	transform.translate(centerImgX, centerImgY);
	transform.rotate(180 * -(rotation - meanRotation) / M_PI);
	if (!selectedFlipProperty.isEmpty() && obj->isSet(selectedFlipProperty.toStdString()) &&
		(selectedFlipValue.isEmpty() || iequals(obj->get(selectedFlipProperty.toStdString()), selectedFlipValue.toStdString()))) {
		if (selectedFlipHorizontally) {
			transform.scale(-1, 1);
		}
		if (selectedFlipVertically) {
			transform.scale(1, -1);
		}
	}
	transform.rotate(rotateDegree);
	transform.scale(scale, scale);
	transform.scale(aspectRatio, 1/aspectRatio);
	transform.shear(xShear, yShear);
	transform.translate(-(centerImgX + borderLeft), -(centerImgY + borderTop));
	transform.translate(xTranslate, yTranslate);
	//QMatrix invTransform = transform.inverted();

	// create a new pixmap for the cropped image and copy the original image in there
	// .. don't forget to take into account the margins
	QPixmap croppedScaledImg((int)roundf(width) + borderLeft + borderRight,
			(int)roundf(height) + borderTop + borderBottom);
	croppedScaledImg.fill(Qt::black);

	// get a 32 bit copy of the original image
	QImage tmpImg;
	if (imgFile.depth() != 32)
		tmpImg = imgFile.convertToFormat(QImage::Format_RGB32, Qt::DiffuseDither);
	else
		tmpImg = imgFile;
	QImage transformedImg = QImage(croppedScaledImg.size(), QImage::Format_RGB32);

	// we will use the OpenCV library .. in order to avoid aliasing
	// prepare two images .. scale the original image down and up, in this way
	// we know that it will be save to use by the QPainter
	IplImage cvImgSrc, cvImgDest;
	cvInitImageHeader(&cvImgSrc, cvSize(imgFile.width(), imgFile.height()), IPL_DEPTH_8U, 4);
	cvImgSrc.imageData = (char*)tmpImg.bits();
	cvInitImageHeader(&cvImgDest, cvSize(croppedScaledImg.width(), croppedScaledImg.height()), IPL_DEPTH_8U, 4);
	cvImgDest.imageData = (char*)transformedImg.bits();

	// downsample the image data if necessary
	if (zoomFactor < 1) {
		IplImage *cvImgSmall = cvCreateImage(cvSize((int)(imgFile.width() * zoomFactor), (int)(imgFile.height() * zoomFactor)), IPL_DEPTH_8U, 4);
		cvResize(&cvImgSrc, cvImgSmall, CV_INTER_AREA);
		cvResize(cvImgSmall, &cvImgSrc, CV_INTER_AREA);
		cvReleaseImage(&cvImgSmall);
	}

	// apply the matrix transformation
	CvMat* cvMat = cvCreateMat(3, 3, CV_32FC1);
	cvmSet(cvMat, 0, 0, transform.m11());
	cvmSet(cvMat, 1, 0, transform.m12());
	cvmSet(cvMat, 2, 0, 0);
	cvmSet(cvMat, 0, 1, transform.m21());
	cvmSet(cvMat, 1, 1, transform.m22());
	cvmSet(cvMat, 2, 1, 0);
	cvmSet(cvMat, 0, 2, transform.dx());
	cvmSet(cvMat, 1, 2, transform.dy());
	cvmSet(cvMat, 2, 2, 1);
	cvWarpPerspective(&cvImgSrc, &cvImgDest, cvMat, CV_INTER_AREA + CV_WARP_FILL_OUTLIERS + CV_WARP_INVERSE_MAP);
	cvReleaseMat(&cvMat);

	// smooth the image
	IplImageWrapper cvImgTmp(cvCloneImage(&cvImgDest));
	if (smoothing > 0.0) {
		cvSmooth(cvImgTmp, &cvImgDest, CV_GAUSSIAN, 0, 0, smoothing);
		cvCopy(&cvImgDest, cvImgTmp);
	}

	// reduce the contrast
	if (contrastReduction != 1.0) {
		cvConvertScale(cvImgTmp, &cvImgDest, contrastReduction);
		cvCopy(&cvImgDest, cvImgTmp);
	}

	// add Gaussian noise
	if (noise > 0.0) {
		addGaussianNoise(cvImgTmp, noise, random);
		cvCopy(cvImgTmp, &cvImgDest);
	}

	// draw the down- and upscaled image
	QPainter p(&croppedScaledImg);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::Antialiasing);
	//p.setMatrix(invTransform);
	p.drawImage(0, 0, transformedImg);

	// if wanted, indicate the original bounding box in the image
	if (drawBoundingBox) {
		QPen pen(QColor(0, 0, 220, 200));
		p.setPen(pen);
		p.drawPolygon(transform.inverted().map(box));
	}
	p.end();

	return croppedScaledImg;
#else
	return QPixmap();
#endif
}

QPixmap ImageExtractDialog::sampleObj(QImage& imgFile, IA::ID objID, bool drawBoundingBox)
{
	// prepare distributions for all the values
	normal_generator_t genXTranslate(random, normal_distribution_t(0, sampleXTranslationSpinBox->value()));
	normal_generator_t genYTranslate(random, normal_distribution_t(0, sampleYTranslationSpinBox->value()));
	normal_generator_t genScale(random, normal_distribution_t(1, sampleScaleSpinBox->value()));
	normal_generator_t genAspectRatio(random, normal_distribution_t(1, sampleAspectRatioSpinBox->value()));
	normal_generator_t genXShear(random, normal_distribution_t(0, sampleXShearSpinBox->value()));
	normal_generator_t genYShear(random, normal_distribution_t(0, sampleYShearSpinBox->value()));
	normal_generator_t genRotation(random, normal_distribution_t(
			sampleRotationMeanSpinBox->value(), sampleRotationSigmaSpinBox->value()));
//	uniform_generator_t genRotation(random, uniform_distribution_t(
//			MIN(sampleRotationSpinBox1->value(), sampleRotationSpinBox2->value()),
//			MAX(sampleRotationSpinBox1->value(), sampleRotationSpinBox2->value()) + 1e-10));
	uniform_generator_t genImgSizeScale(random, uniform_distribution_t(1, sampleSizeSpinBox->value() + 1e-10));
	normal_generator_t genSmoothing(random, normal_distribution_t(0, sampleSmoothingSpinBox->value()));
	normal_generator_t genContrastReduction(random, normal_distribution_t(1, sampleContrastSpinBox->value()));
	normal_generator_t genNoise(random, normal_distribution_t(0, sampleNoiseSpinBox->value()));

	// generate a new sample based on the given object ID
	double tmpSmoothing = genSmoothing();
	double tmpNoise = genNoise();
	return cropObj(imgFile, objID, drawBoundingBox,
			genXTranslate(), genYTranslate(), genScale(), genAspectRatio(),
			genXShear(), genYShear(), genRotation(), genImgSizeScale(),
			MAX(tmpSmoothing, -tmpSmoothing), genContrastReduction(), MAX(tmpNoise, -tmpNoise));
}

int ImageExtractDialog::exec()
{
	fillAnnotationData();
	return QDialog::exec();
}

void ImageExtractDialog::on_addConstraintButton_clicked()
{
	propertiesTableWidget->setRowCount(propertiesTableWidget->rowCount() + 1);
}

void ImageExtractDialog::on_removeConstraintsButton_clicked()
{
	// iterate through the rows and delete those rows which are selected
	for (int i = 0; i < propertiesTableWidget->rowCount(); i++) {
		QTableWidgetItem *item = propertiesTableWidget->item(i, 0);
		if (NULL != item && item->isSelected()) {
			propertiesTableWidget->removeRow(i);
			--i;
		}
	}
}

void ImageExtractDialog::on_addNotConstraintButton_clicked()
{
	notPropertiesTableWidget->setRowCount(notPropertiesTableWidget->rowCount() + 1);
}

void ImageExtractDialog::on_removeNotConstraintsButton_clicked()
{
	// iterate through the rows and delete those rows which are selected
	for (int i = 0; i < notPropertiesTableWidget->rowCount(); i++) {
		QTableWidgetItem *item = notPropertiesTableWidget->item(i, 0);
		if (NULL != item && item->isSelected()) {
			notPropertiesTableWidget->removeRow(i);
			--i;
		}
	}
}

void ImageExtractDialog::on_saveAlignmentButton_clicked()
{
	// ask the user to add files
	QString file = QFileDialog::getSaveFileName(
 			this,
			"Save Alignment as ...",
			lastOpenedDir,
			"Annotation Database (*.annotation)");

	if (file.isEmpty())
		return;

	// check wether an extension has been given
	if (!file.section('/', -1).contains('.'))
		file += ".annotation";
	
	// create a annotations file with all files and the aligned objects
	IA::ImgAnnotations alignments;
	BOOST_FOREACH(const std::string& str, orgAnnotations->getFilePaths()) {
		QString imgFilePath = QString::fromStdString(str);

		// get the absolute path to the image file
		if (imgFilePath[0] != '/')
			imgFilePath = *orgDatabasePath + "/" + imgFilePath;

		// add file to alignments data base
		alignments.addFile(imgFilePath.toStdString());
	}
	BOOST_FOREACH(ID objID, filteredObjIDs2) {
		Object *obj = orgAnnotations->getObject(objID);
		Q_CHECK_PTR(obj);

		// get the image file
		QString imgFilePath = QString::fromStdString(obj->getFilePath());
	
		// get the absolute path to the image file
		if (imgFilePath[0] != '/')
			imgFilePath = *orgDatabasePath + "/" + imgFilePath;

		// create a new object and assign the aligned bounding box
		Object* newObj = alignments.newObject(imgFilePath.toStdString());
		*newObj = *obj;
		QRectF box = getCropRect(objID);
		newObj->set("bbox", rect2str(box).toStdString());
		
		// get the original bounding box and the fix points
		box = str2rect(QString::fromStdString(obj->get("bbox")));
		QList<QPointF> fixPoints = str2points(QString::fromStdString(obj->get("fixpoints")));

		// compute the selectedCenter point of the fix points
		QPointF centerFixPoints;
		centerFixPoints.rx() = 0;
		centerFixPoints.ry() = 0;
		if (fixPoints.count() > 0) {
			for (int iFixPoint = 0; iFixPoint < fixPoints.count(); iFixPoint++)
				centerFixPoints += fixPoints[iFixPoint];
			centerFixPoints.rx() /= fixPoints.count();
			centerFixPoints.ry() /= fixPoints.count();
		}

		// set the selectedCenter point of the object
		QPointF centerPoint;
		if (selectedCenter == FixPointsCenter)
			centerPoint = centerFixPoints;
		else
			centerPoint = box.center();
		
		// save the center point
		QList<QPointF> tmpPoints;
		tmpPoints.append(centerPoint);
		newObj->set("center", points2str(tmpPoints).toStdString());
	}

	// save the data and chage the cursor to a waiting cursor
	alignments.saveToFile(file.toStdString());
}

void ImageExtractDialog::on_saveButton_clicked()
{
	QFileDialog fd(this);
	fd.setFileMode(QFileDialog::AnyFile);
	if (lastOpenedDir.isEmpty() || lastOpenedDir == "/")
		fd.setDirectory(QDir::home());
	else
		fd.setDirectory(lastOpenedDir.section("/", 0, -2));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	fd.setConfirmOverwrite(true);

	//QString openedDir = QFileDialog::getExistingDirectory(this, "Choose a directory to save the images", lastOpenedDir);
	if (!fd.exec())
		return;
	if (fd.selectedFiles().count() == 0)
		return;
	QString openedDir = fd.selectedFiles().first();
	if (openedDir.isEmpty() || QFile::exists(openedDir))
		return;
	fd.directory().mkdir(openedDir);

	// save this directory for the next time
	lastOpenedDir = openedDir;

//	// save an information file with the used options
//	QFile file(openedDir + "/" + "extract_options.txt");
//	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//		QTextStream out(&file);
//		out << "These images have been extracted with the following options:\n\n";
//		out << "containing properties:\n";
//		BOOST_FOREACH(StrStrPair entry, selectedProperties)
//			out << "\t" << QString::fromStdString(entry.first) << ": " << QString::fromStdString(entry.second) << "\n";
//		out << "not containing properties:\n";
//		BOOST_FOREACH(StrStrPair entry, selectedNotProperties)
//			out << "\t" << QString::fromStdString(entry.first) << ": " << QString::fromStdString(entry.second) << "\n";
//		if (selectedNumOfFixPoints >= 0)
//			out << "number of fix points: " << selectedNumOfFixPoints << "\n";
//		if (selectedMinDimension >= 0)
//			out << "min dimension: " << selectedMinDimension << "\n";
//		if (selectedMaxDimension >= 0)
//			out << "max dimension: " << selectedMaxDimension << "\n";
//
//		out << "centering: ";
//		if (FixPointsCenter == selectedCenter)
//			out << "center of fix points";
//		else
//			out << "center of bounding box";
//		out << "\n";
//
//		out << "scaling: ";
//		if (StdDevFixPointsScale == selectedScale)
//			out << "standard deviation of fix points to center of fix points";
//		if (WidthScale == selectedScale)
//			out << "width of bounding box";
//		if (HeightScale == selectedScale)
//			out << "height of bounding box";
//		else
//			out << "longest side of bounding box";
//		out << "\n";
//
//		if (!selectedFlipProperty.isEmpty()) {
//			out << "flip property: " << selectedFlipProperty << "\n";
//			out << "flip value: " << selectedFlipValue << "\n";
//			out << "flip direction: ";
//			if (selectedFlipVertically && selectedFlipHorizontally)
//				out << "horizontally + vertically";
//			else if (selectedFlipHorizontally)
//				out << "horizontally";
//			else if (selectedFlipVertically && selectedFlipHorizontally)
//				out << "vertically";
//			out << "\n";
//		}
//
//		out << "output image (width, height): "
//				<< widthSpinBox->value() << ", "
//				<< heightSpinBox->value() << "\n";
//		out << "margins (left, top, right, bottom): "
//				<< leftBorderSpinBox->value() << ", "
//				<< topBorderSpinBox->value() << ", "
//				<< rightBorderSpinBox->value() << ", "
//				<< bottomBorderSpinBox->value() << "\n";
//
//		if (rotateSpinBox->value() > 0 || numOfRotationsSpinBox->value() > 0
//				|| scaleSpinBox->value() > 0 || numOfScalesSpinBox->value() > 0)
//			out << "generate duplicates (maxRotateDegree, numOfRotations, maxScale, numOfScales): "
//					<< rotateSpinBox->value() << ", "
//					<< numOfRotationsSpinBox->value() << ", "
//					<< scaleSpinBox->value() << ", "
//					<< numOfScalesSpinBox->value() << "\n";
//	}

	// init the progressBar
	progressBar->setEnabled(true);
	progressBar->setMaximum(filteredObjIDs2.size());
	progressBar->setValue(0);

	// go through all objects and save them
	int countAll = 1;
	QString lastImgFilePath;
	QImage imgFile;
	int nSamplesPerObj = samplesPerObjSpinBox->value();
	BOOST_FOREACH(ID objID, filteredObjIDs2) {
		Object *obj = orgAnnotations->getObject(objID);
		Q_CHECK_PTR(obj);

		// load the image file
		QString imgFilePath = QString::fromStdString(obj->getFilePath());
		QString imgFileName = QString::fromStdString(ImgAnnotations::fileName(obj->getFilePath()));
		if (lastImgFilePath != imgFilePath)
			imgFile = loadImg(imgFilePath);

		// sample n new objects
		for (int i = 0; i < nSamplesPerObj; ++i) {
			// sample a new image
			QPixmap sampledImg = sampleObj(imgFile, objID);

			// save the image
			QString filePath = openedDir + "/" + imgFileName.section('.', 0, -2)
					+ "_" + QString::number(objID) + "_" + QString::number(i) + ".png";
			sampledImg.save(filePath, "PNG");
		}

//		// save the rotated and scaled duplicates
//		for (int iRotate = -1 * numOfRotations; iRotate <= numOfRotations; iRotate++) {
//			for (int iScale = -1 * numOfScales; iScale <= numOfScales; iScale++) {
//				// crop the scaled and rotated version of the image
//				double rotate = iRotate * rotateStep;
//				double scale = 1 + iScale * scaleStep;
//				QPixmap croppedImg = cropObj(imgFile, objID, rotate, scale);
//
//				// save the image
//				QString filePath = openedDir + "/" + imgFileName.section('.', 0, -2) + "_" + QString::number(objID);
//				if (0 != iScale) {
//					filePath += "_scale" + QString::number(scale);
//				}
//				if (iRotate < 0)
//					filePath += "_rotateNeg" + QString::number(-1 * rotate);
//				else if (iRotate > 0)
//					filePath += "_rotatePos" + QString::number(rotate);
//				filePath += ".png";
//				croppedImg.save(filePath, "PNG");
//			}
//		}

		// update the progressBar
		progressBar->setValue(countAll++);
	}

	// reset the progressBar
	progressBar->setMaximum(1);
	progressBar->setValue(0);
	progressBar->setEnabled(false);
}

void ImageExtractDialog::on_applyButton_clicked()
{
	// check how many fix points the object should have
	int nFixPoints = -1;
	if (Qt::Checked == numOfFixPointsCheckBox->checkState())
		nFixPoints = numOfFixPointsSpinBox->value();

	// check wether everything is set rightnFixPoints
	int newCenter = centerComboBox->currentIndex();
	int newScale = scaleNormComboBox->currentIndex();
	if (newCenter == FixPointsCenter && nFixPoints < 1) {
		QMessageBox::information(this,
				"Image Extraction",
				"If you want to use the selectedCenter of the fix points,\n"
				"you need at least one fix point!",
				QMessageBox::Ok);
		return;
	}
	if (newScale == StdDevFixPointsScale && nFixPoints < 2) {
		QMessageBox::information(this,
				"Image Extraction",
				"If you want to use the.scale information given through\n"
				"the fix points, you need at least two fix points!",
				QMessageBox::Ok);
		return;
	}

	// update internal variables
	selectedCenter = (Center)newCenter;
	selectedScale = (Scale)newScale;

	// check whether flipping is performed
	selectedFlipVertically = false;
	selectedFlipHorizontally = false;
	selectedFlipProperty = flipPropertyLineEdit->text().trimmed();
	selectedFlipValue = flipValueLineEdit->text().trimmed();
	if (!selectedFlipProperty.isEmpty()) {
		if (flipDirectionComboBox->currentIndex() == 0)
			selectedFlipHorizontally = true;
		else if (flipDirectionComboBox->currentIndex() == 1)
			selectedFlipVertically = true;
		else if (flipDirectionComboBox->currentIndex() == 2) {
			selectedFlipHorizontally = true;
			selectedFlipVertically = true;
		}
	}

	// check whether nomalization in rotation is performed
	normalizeRotation = false;
	if (nFixPoints > 0 && rotationNormComboBox1->currentIndex() > 0 && rotationNormComboBox2->currentIndex() > 0) {
		int index1 = rotationNormComboBox1->currentIndex();
		int index2 = rotationNormComboBox2->currentIndex();
		if (index1 == index2) {
			QMessageBox::information(this,
					"Image Extraction",
					"If you want a normalization of the rotation using\n"
					"the fix points, you need to choose two differnt points!",
					QMessageBox::Ok);
			return;
		}
		if (MAX(index1, index2) - 2 > nFixPoints) {
			QMessageBox::information(this,
					"Image Extraction",
					"For the normalization of the rotation,\n"
					"chose a fix point that does exist!",
					QMessageBox::Ok);
			return;
		}
		normalizeRotation = true;
	}

	// filter the objects, compute the alignment data and refresh the view
	filterData();
	computeAlignment();
	refreshFilteredObjView();
	if (heightRadioButton->isChecked())
		computeWidth();
	else
		computeHeight();
	previewGroupBox->setTitle("Output Preview (" + QString::number(filteredObjTreeWidget->topLevelItemCount()) + " objects)");
}

void ImageExtractDialog::on_generateSamplesButton_clicked()
{
	// get the current selected image
	QTreeWidgetItem *currentItem = filteredObjTreeWidget->currentItem();
	if (!currentItem)
		return;

	// check wether dir/file/object have been selected
	ID objID = static_cast<ID>(currentItem->text(3).toInt());
	Object *obj = orgAnnotations->getObject(objID);
	Q_CHECK_PTR(obj);
	QString imgFilePath = QString::fromStdString(obj->getFilePath());

	// get the cropped object
	QImage imgFile = loadImg(imgFilePath);
	QPixmap objImg = sampleObj(imgFile, objID, true);

	// set the cropped and scaled object from the image as new pixmap
	if (!objImg.isNull())
		pixmapWidget->setPixmap(objImg);
}

void ImageExtractDialog::on_widthSpinBox_valueChanged(int newValue)
{
	if (widthRadioButton->isChecked())
		computeHeight();
}

void ImageExtractDialog::on_heightSpinBox_valueChanged(int newValue)
{
	if (heightRadioButton->isChecked())
		computeWidth();
}

void ImageExtractDialog::on_filteredObjTreeWidget_currentItemChanged(QTreeWidgetItem *currentItem)
{
	printf("Checkpoint 0\n");
	if (!currentItem)
		return;

	// check wether dir/file/object have been selected
	ID objID = static_cast<ID>(currentItem->text(3).toInt());
	Object *obj = orgAnnotations->getObject(objID);
	Q_CHECK_PTR(obj);
	QString imgFilePath = QString::fromStdString(obj->getFilePath());

	// get the cropped object
	QImage imgFile = loadImg(imgFilePath);
	QPixmap objImg = cropObj(imgFile, objID, true);

	// set the cropped and scaled object from the image as new pixmap
	if (!objImg.isNull())
		pixmapWidget->setPixmap(objImg);
}

void ImageExtractDialog::nextPreviousFile(MainWindow::Direction direction)
{
	// choose the current items from the imgTreeWidget
	QTreeWidgetItem *current = filteredObjTreeWidget->currentItem();
	if (!current)
		return;

	// get the index
	int iCurrent = filteredObjTreeWidget->indexOfTopLevelItem(current);
	if (iCurrent < 0)
		return;

	// select the next file index
	if (direction == MainWindow::Up)
		iCurrent--;
	else
		iCurrent++;

	// the index may be negative or too big
	if (iCurrent < 0)
		iCurrent = 0;
	else if (iCurrent >= filteredObjTreeWidget->topLevelItemCount())
		iCurrent = filteredObjTreeWidget->topLevelItemCount() - 1;

	// we handled all special cases thus we may try to set the next current item
	current = filteredObjTreeWidget->topLevelItem(iCurrent);
	if (current)
		filteredObjTreeWidget->setCurrentItem(current);
}

void ImageExtractDialog::refreshFilteredObjView()
{
	// save the currently viewed object id
	ID objID = -1;
	QTreeWidgetItem *currentItem = filteredObjTreeWidget->currentItem();
	if (NULL != currentItem) {
		objID = static_cast<ID>(currentItem->text(3).toInt());
	}

	// clear the filteredObjTreeWidget
	filteredObjTreeWidget->blockSignals(true);
	filteredObjTreeWidget->setUpdatesEnabled(false);
	filteredObjTreeWidget->clear();

	// loop through the filtered objects and add them to the filteredObjTreeWidget
	QList<QTreeWidgetItem*> newItems;
	QString tmpStr;
	BOOST_FOREACH(ID objID, filteredObjIDs2) {
		Object *obj = orgAnnotations->getObject(objID);
		Q_CHECK_PTR(obj);

		// get the dir path and the filename
		QString imgDirPath = QString::fromStdString(ImgAnnotations::dirPath(obj->getFilePath()));
		QString imgFileName = QString::fromStdString(ImgAnnotations::fileName(obj->getFilePath()));

		QTreeWidgetItem *newItem = new QTreeWidgetItem();
		newItem->setText(0, QString::fromStdString(obj->get("type")));
		newItem->setText(1, imgFileName);
		newItem->setText(2, imgDirPath);
		tmpStr.setNum(objID);
		newItem->setText(3, tmpStr);
		newItems.append(newItem);
	}
	filteredObjTreeWidget->addTopLevelItems(newItems);

	// resize the columns such that they fit the data
	filteredObjTreeWidget->resizeColumnToContents(0);
	filteredObjTreeWidget->resizeColumnToContents(1);
	filteredObjTreeWidget->resizeColumnToContents(2);

	// sort the elements
	filteredObjTreeWidget->sortItems(1, Qt::AscendingOrder);
	filteredObjTreeWidget->sortItems(2, Qt::AscendingOrder);

	// unblock tree view
	filteredObjTreeWidget->blockSignals(false);
	filteredObjTreeWidget->setUpdatesEnabled(true);

	// now choose the right item as current viewed item
	QTreeWidgetItem* foundItem = NULL;
	QString idStr = QString::number(objID);
	for (int i = 0; i < filteredObjTreeWidget->topLevelItemCount() && foundItem == NULL; ++i) {
		QTreeWidgetItem* tmpItem = filteredObjTreeWidget->topLevelItem(i);
		if (NULL != tmpItem && tmpItem->text(3) == idStr)
			foundItem = tmpItem;
	}
	if (NULL != foundItem) {
		filteredObjTreeWidget->setCurrentItem(foundItem);
	}
}

void ImageExtractDialog::computeWidth()
{
	// compute the optimal height given a width
	double height = heightSpinBox->value();
	double width = 0;
	if (normMaxBottom - normMinTop != 0)
		width = ((normMaxRight - normMinLeft) / (normMaxBottom - normMinTop)) * height;
	widthSpinBox->setValue((int)roundf(width));
	updateOutputSizeLabel();
}

void ImageExtractDialog::computeHeight()
{
	// compute the optimal height if necessary
	double width = widthSpinBox->value();
	double height = 0;
	if (normMaxRight - normMinLeft != 0)
		height = ((normMaxBottom - normMinTop) / (normMaxRight - normMinLeft)) * width;
	heightSpinBox->setValue((int)roundf(height));
	updateOutputSizeLabel();
}

void ImageExtractDialog::updateOutputSizeLabel()
{
	outputSizeLabel->setText(QString::number(widthSpinBox->value() + leftBorderSpinBox->value() + rightBorderSpinBox->value()) 
			+ "x" + QString::number(heightSpinBox->value() + topBorderSpinBox->value() + bottomBorderSpinBox->value()));
}

void ImageExtractDialog::onWheelTurnedInScrollArea(QWheelEvent *event)
{
	if (!event->isAccepted()) {
		// select a different file
		if (event->delta() < 0)
			nextPreviousFile(MainWindow::Down);
		else if (event->delta() > 0)
			nextPreviousFile(MainWindow::Up);
		event->accept();
	}
}

void ImageExtractDialog::wheelEvent(QWheelEvent *event)
{
}


