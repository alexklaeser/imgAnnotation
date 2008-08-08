#ifndef ImageExtractDialog_H
#define ImageExtractDialog_H

#include <QDialog>
#include <map>
#include <boost/random.hpp>
#include "ui_ImageExtractDialog.h"
#include "MainWindow.h"

class PixmapWidget;
class IA::ImgAnnotations;


class ImageExtractDialog : public QDialog, private Ui::ImageExtractionDialog
{
	Q_OBJECT

public:
//	enum Direction { Up, Down };
	typedef std::multimap<std::string, std::string> StrStrMultiMap;

private:
	enum Scale { MaxSideScale, HeightScale, WidthScale, StdDevFixPointsScale };
	enum Center { BoundingBoxCenter, FixPointsCenter };

	// random generator
	typedef boost::mt19937 random_generator_t;
	typedef boost::normal_distribution<double> normal_distribution_t;
	typedef boost::uniform_real<double> uniform_distribution_t;
	typedef boost::variate_generator<random_generator_t&, normal_distribution_t> normal_generator_t;
	typedef boost::variate_generator<random_generator_t&, uniform_distribution_t> uniform_generator_t;
	random_generator_t random;

	IA::ImgAnnotations *orgAnnotations;
	QString *orgDatabasePath;
	IA::IDList filteredObjIDs;
	IA::IDList filteredObjIDs2;
	PixmapWidget *pixmapWidget;
	QString lastOpenedDir;
	double normMinTop, normMaxBottom, normMinLeft, normMaxRight, meanRotation;

	Center selectedCenter;
	Scale selectedScale;
	StrStrMultiMap selectedProperties, selectedNotProperties;
	StrStrMultiMap selectedProperties2, selectedNotProperties2;
	QString selectedObjType;
	QString selectedFlipProperty, selectedFlipValue;
	int selectedNumOfFixPoints;
	int selectedMinDimension, selectedMaxDimension;
	bool selectedFlipHorizontally, selectedFlipVertically;
	bool normalizeRotation;

public:
	ImageExtractDialog(IA::ImgAnnotations *, QString *, QWidget *parent = 0, Qt::WFlags flags = 0);

protected:
	void wheelEvent(QWheelEvent *event);

private:
	void fillAnnotationData();
	void filterData();
	void computeAlignment();
	void refreshFilteredObjView();
	QImage loadImg(QString);
	QPixmap sampleObj(QImage&, IA::ID, bool drawBoundingBox = false);
	QRectF getCropRect(IA::ID);
	QPixmap cropObj(QImage&, IA::ID, bool drawBoundingBox = false,
			double xTranslate = 0, double yTranslate = 0, double scale = 1,
			double aspectRatio = 1, double xShear = 0, double yShear = 0, double rotateDegree = 0, double imgSizeScale = 1,
			double smoothing = 0, double contrastReduction = 1, double noise = 0);

public slots:
	int exec();
	void on_applyButton_clicked();
	void on_saveButton_clicked();
	void on_saveAlignmentButton_clicked();
	void on_generateSamplesButton_clicked();
	void on_widthSpinBox_valueChanged(int);
	void on_heightSpinBox_valueChanged(int);
	void on_filteredObjTreeWidget_currentItemChanged(QTreeWidgetItem*);
	void on_addConstraintButton_clicked();
	void on_removeConstraintsButton_clicked();
	void on_addNotConstraintButton_clicked();
	void on_removeNotConstraintsButton_clicked();
	void updateOutputSizeLabel();

private slots:
	void onWheelTurnedInScrollArea(QWheelEvent *);
	void nextPreviousFile(MainWindow::Direction);
	void computeWidth();
	void computeHeight();
};

#endif
