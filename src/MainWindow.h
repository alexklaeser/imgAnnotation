#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QRectF>
#include <QCloseEvent>
#include <string>
#include <ImgAnnotations.h>
#include "ui_MainWindow.h"
#include "ScrollAreaNoWheel.h"

class ImageExtractDialog;
class AnnotationsPixmapWidget;
class QTableWidgetItem;
class QWheelEvent;


class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:
	enum Direction { Up, Down };

private:
	AnnotationsPixmapWidget *pixmapWidget;
	ScrollAreaNoWheel *scrollArea;
	QString lastDirAddImg, lastDatabasePath, databasePath;

	IA::ImgAnnotations annotations;
	QStringList propertyList, reservedProperties;

	IA::Object *copiedObj;

	ImageExtractDialog *imageExtractDialog;

private:
	IA::ID objectIDAtRow(int iRow) const;
	IA::ID objectIDFromItem(QTableWidgetItem *item) const;
	IA::IDList selectedObjectIDs() const;
	void setCurrentTableItem(IA::ID objID);

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	QString currentFilePath() const;
	QString currentDirPath() const;
	IA::ID currentObjectID() const;
	IA::Object *currentObject() const;
	IA::IDList fileObjectIDs() const;

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent * event);
	void keyReleaseEvent(QKeyEvent * event);
	void wheelEvent(QWheelEvent *event);

public slots:
//	void on_pixmapWidget_objectContentChanged(IA::ID);
//	void onWheelTurnedInScrollArea(QWheelEvent *);
	void on_actionOpenDatabase_triggered();
	void on_actionSaveDatabase_triggered();
	void on_actionSaveDatabaseAs_triggered();
	void on_actionQuit_triggered();
	void on_actionShortcutHelp_triggered();
	void on_actionCopyObj_triggered();
	void on_actionPasteObj_triggered();
	void on_actionExtractImages_triggered();
	void on_addImgButton_clicked();
	void on_delImgButton_clicked();
	void on_addObjButton_clicked();
	void on_delObjButton_clicked();
	void on_addPropertyButton_clicked();
//	void on_propertiesTableWidget_itemChanged();
//	void on_objTypeComboBox_currentIndexChanged(const QString &);
	void on_imgTreeWidget_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
	void on_objTableWidget_itemSelectionChanged();
	void on_objTableWidget_currentItemChanged(QTableWidgetItem *, QTableWidgetItem *);
	void on_objTableWidget_itemChanged(QTableWidgetItem*);
	void onPixmapWidgetActiveObjectChanged(IA::ID);
	void onScrollAreaWheelTurned(QWheelEvent*);
	void refreshImgView();
	void refreshObjView();
//	void refreshPropertiesView();
	void nextPreviousFile(Direction);

signals:
	void activeObjectChanged(IA::ID objID);
	void activeFileChanged(const QString& filePath);
	void selectedObjectsChanged(const IA::IDList& objIDs);
};

#endif
