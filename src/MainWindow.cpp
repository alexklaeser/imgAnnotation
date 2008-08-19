#include "MainWindow.h"
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <stdio.h>
#include <boost/foreach.hpp>
#include "functions.h"
#include "ScrollAreaNoWheel.h"
#include "ImageExtractDialog.h"
#include "AnnotationsPixmapWidget.h"

using namespace IA;
using std::string;


MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	// set up the GUI
	setupUi(this);
	imageExtractDialog = new ImageExtractDialog(&annotations, &lastDatabasePath, this);
	scrollArea = new ScrollAreaNoWheel(this);
	pixmapWidget = new AnnotationsPixmapWidget(&annotations, scrollArea, scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(pixmapWidget);
	setCentralWidget(scrollArea);
//	propertiesTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	// theses properties cannot be used/edited/seen directly by the user in the table view
	reservedProperties << "bbox" << "fixpoints";
	copiedObj = NULL;

	// we want to receive key events, therefore we have to set the focus policy
	setFocusPolicy(Qt::WheelFocus);

	// make some connections
	connect(zoomSpinBox, SIGNAL(valueChanged(double)), pixmapWidget, SLOT(setZoomFactor(double)));
//	connect(this, SIGNAL(filesChanged()), this, SLOT(refreshImgView()));
//	connect(this, SIGNAL(objectsChanged()), this, SLOT(refreshObjView()));
	connect(pixmapWidget, SIGNAL(zoomFactorChanged(double)), zoomSpinBox, SLOT(setValue(double)));
	connect(pixmapWidget, SIGNAL(activeObjectChanged(IA::ID)), this, SLOT(onPixmapWidgetActiveObjectChanged(IA::ID)));
	connect(this, SIGNAL(activeObjectChanged(IA::ID)), pixmapWidget, SLOT(onActiveObjectChanged(IA::ID)));
	connect(this, SIGNAL(selectedObjectsChanged(const IA::IDList &)), pixmapWidget, SLOT(onVisibleObjectsChanged(const IA::IDList &)));
	connect(scrollArea, SIGNAL(wheelTurned(QWheelEvent*)), this, SLOT(onScrollAreaWheelTurned(QWheelEvent *)));
}

QString MainWindow::currentDirPath() const
{
	QTreeWidgetItem *current = imgTreeWidget->currentItem();
	if (!current || !current->parent())
		return "";

	return current->parent()->text(0);
}

QString MainWindow::currentFilePath() const
{
	QTreeWidgetItem *current = imgTreeWidget->currentItem();
	if (NULL == current || NULL == current->parent())
		return "";

	return currentDirPath() + "/" + current->text(0);
}

ID MainWindow::currentObjectID() const
{
	// hand the active object over to the PixmapWidget
	return objectIDAtRow(objTableWidget->currentRow());
}

Object *MainWindow::currentObject() const
{
	return annotations.getObject(currentObjectID());
}

IA::ID MainWindow::objectIDFromItem(QTableWidgetItem *item) const
{
	if (NULL == item)
		return -1;
	return objectIDAtRow(item->row());
}

IA::ID MainWindow::objectIDAtRow(int iRow) const
{
	// query the header item of the given row, it holds the object id
	QTableWidgetItem *item = objTableWidget->item(iRow, 0);
	if (NULL == item)
		return -1;

	return static_cast<ID>(item->text().toInt());
}

void MainWindow::setCurrentTableItem(ID objID)
{
	// find the item in the TableWidget with the correcht object id
	QTableWidgetItem* foundItem = NULL;
	QString idStr = QString::number(objID);
	for (int i = 0; i < objTableWidget->rowCount() && foundItem == NULL; ++i) {
		QTableWidgetItem* tmpItem = objTableWidget->item(i, 0);
		if (NULL != tmpItem && tmpItem->text() == idStr)
			foundItem = tmpItem;
	}
	if (NULL != foundItem) {
		objTableWidget->clearSelection();
		objTableWidget->setCurrentItem(foundItem);
	}
}

//void MainWindow::onObjectContentChanged(ID objID)
//{
//	// check wether dir/file/object have been selected
//	QString iFile = currentFile();
//	QString iDir = currentDir();
//	int iObj = currentObj();
//	if (iFile.isEmpty() || iDir.isEmpty() || iObj < 0)
//		return;
//
//	// set the new bounding box values
//	IAObj *obj = annotations.getObject(iDir, iFile, iObj);
//	if (obj) {
//		obj->box = newBox.box;
//		obj->fixPoints = newBox.fixPoints;
//	}
//}

void MainWindow::onPixmapWidgetActiveObjectChanged(ID objID)
{
	// if the id is < 0, clear the selection
	if (objID < 0) {
		objTableWidget->setCurrentItem(NULL);
		objTableWidget->clearSelection();
	}
	// otherwise find the correct item in the QTableWidget
	else {
		for (int i = 0; i < objTableWidget->rowCount(); i++) {
			QTableWidgetItem *item = objTableWidget->item(i, 0);
			ID tmpID = objectIDFromItem(item);
			if (tmpID == objID) {
				// we found the right item
				objTableWidget->blockSignals(true);
				objTableWidget->clearSelection();
				objTableWidget->setCurrentCell(i, 0);
				objTableWidget->blockSignals(false);
				break;
			}
		}
	}
}

void MainWindow::onScrollAreaWheelTurned(QWheelEvent *event)
{
	wheelEvent(event);
}

void MainWindow::on_actionOpenDatabase_triggered()
{
	// clear the status bar and set the normal mode for the pixmapWidget
	pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
	statusBar()->clearMessage();

	// ask the user to add files
	QString file = QFileDialog::getOpenFileName(
 			this,
			"Open an Existing Database",
			lastDatabasePath,
			"Annotation Database (*.annotation)");

	if (file.isEmpty())
		return;

	// load the annotation file
	annotations.loadFromFile(file.toStdString());

	// create a list with all properties
	propertyList = std2qt(annotations.getAllProperties());
	propertyList.prepend("<ID>");
	foreach (QString property, reservedProperties)
		propertyList.removeAll(property);

	// update the window title
	setWindowTitle("ImageAnnotation - " + file.section('/', -1));

	// update the statusbar
	statusBar()->showMessage("Opened database file " + file, 5 * 1000);

	// save the last filepath
	lastDatabasePath = file.section('/', 0, -2);
	databasePath = file;

	refreshImgView();
}

void MainWindow::on_actionSaveDatabase_triggered()
{
	// clear the status bar and set the normal mode for the pixmapWidget
	pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
	statusBar()->showMessage("Saving database to file " + databasePath);

	if (databasePath.isEmpty()) {
		// no filename given, launch "save as"
		on_actionSaveDatabaseAs_triggered();
		return;
	}

	// save the data and chage the cursor to a waiting cursor
	QApplication::setOverrideCursor(Qt::WaitCursor);
	annotations.saveToFile(databasePath.toStdString());
	QApplication::restoreOverrideCursor();

	// update the statusbar
	statusBar()->showMessage("Saved database to file " + databasePath, 5 * 1000);
}

void MainWindow::on_actionSaveDatabaseAs_triggered()
{
	// clear the status bar and set the normal mode for the pixmapWidget
	pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
	statusBar()->clearMessage();

	// ask the user to add files
	QString file = QFileDialog::getSaveFileName(
 			this,
			"Save Database as ...",
			lastDatabasePath,
			"Annotation Database (*.annotation)");

	if (file.isEmpty())
		return;

	// check wether an extension has been given
	if (!file.section('/', -1).contains('.'))
		file += ".annotation";

	// save the data and chage the cursor to a waiting cursor
	QApplication::setOverrideCursor(Qt::WaitCursor);
	annotations.saveToFile(file.toStdString());
	QApplication::restoreOverrideCursor();

	// update the window title
	setWindowTitle("ImageAnnotation - " + file.section('/', -1));

	// update the statusbar
	statusBar()->showMessage("Saved database to file " + file, 5 * 1000);

	// save the last database path
	lastDatabasePath = file.section('/', 0, -2);
	databasePath = file;
}

void MainWindow::on_actionQuit_triggered()
{
	close();
}

void MainWindow::on_actionShortcutHelp_triggered()
{
	// clear the status bar and set the normal mode for the pixmapWidget
	pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
	statusBar()->clearMessage();

	// we display an overview on shortcuts
 	QMessageBox::about(this, "Shortcut Help",
			"<table border=0 cellpadding=0 cellspacing=2>\n"
			"<tr>\n"
				"<td><b>Insert</b></td>\n"
				"<td width=10></td>\n"
				"<td>create a new object</td>\n"
			"</tr><tr>\n"
				"<td><b>Delete</b></td>\n"
				"<td width=10></td>\n"
				"<td>delete the current object</td>\n"
			"</tr><tr>\n"
				"<td><b>Ctrl+MouseWheel</b></td>\n"
				"<td></td>\n"
				"<td>zoom in/out</td>\n"
			"</tr><tr>\n"
				"<td><b>Ctrl+MouseClick</b></td>\n"
				"<td></td>\n"
				"<td>create/delete a new fix point in the current object</td>\n"
			"</tr><tr>\n"
				"<td><b>Shift+MouseMove</b></td>\n"
				"<td></td>\n"
				"<td>browsing mode, i.e. select an object visually</td>\n"
			"</tr>\n"
			"</table>\n");
}

void MainWindow::on_actionCopyObj_triggered()
{
	Object *obj = currentObject();
	if (NULL != obj) {
		if (NULL != copiedObj)
			delete copiedObj;
		copiedObj = new Object("", -1);
		*copiedObj = *obj;
		statusBar()->showMessage("Object copied", 2 * 1000);
	}
}

void MainWindow::on_actionPasteObj_triggered()
{
	if (NULL == copiedObj || currentFilePath().isEmpty())
		return;

	Object *newObj = annotations.newObject(currentFilePath().toStdString());
	*newObj = *copiedObj;
	statusBar()->showMessage("Copied object pasted", 2 * 1000);

	// refresh the TableWidget for the objects
	refreshObjView();

	// choose the new object as the current object
	setCurrentTableItem(newObj->getID());

	// refresh
//	emit selectedObjectsChanged(fileObjectIDs());
//	pixmapWidget->repaint();
}

void MainWindow::on_actionExtractImages_triggered()
{
	// clear the status bar and set the normal mode for the pixmapWidget
	pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
	statusBar()->clearMessage();

	// start the dialog
	imageExtractDialog->exec();
}

void MainWindow::on_addImgButton_clicked()
{
	// ask the user to add files
	QStringList files = QFileDialog::getOpenFileNames(
 			this,
			"Add Files to the Data Base",
			lastDirAddImg,
			"Images (*.png *.jpg)");

	if (files.size() <= 0)
		return;

	// add files to the data structure
	annotations.addFiles(qt2std(files));

	// save the last opened dir
	lastDirAddImg = files[0].section('/', 0, -2);

	refreshImgView();
}

void MainWindow::on_delImgButton_clicked()
{
	// ask user for confirmation
	if (QMessageBox::Yes != QMessageBox::question(
			this,
			"Removing Images from DB",
			"Are you sure that you would like to remove"
			"the selected files from the database?",
			QMessageBox::Yes, QMessageBox::No))
		return;

	// go through all directories and files and collect the selected files
	QStringList selectedFiles;
	for (int i = 0; i < imgTreeWidget->topLevelItemCount(); i++) {
		QTreeWidgetItem *currentDir = imgTreeWidget->topLevelItem(i);

		// go through all files associated with the directory
		for (int j = 0; j < currentDir->childCount(); j++) {
			QTreeWidgetItem *currentFile = currentDir->child(j);
			if (imgTreeWidget->isItemSelected(currentDir) || imgTreeWidget->isItemSelected(currentFile))
				// add the filepath to the list
				selectedFiles << currentDir->text(0) + "/" + currentFile->text(0);
		}
	}

	// remove selected files from the database
	annotations.removeFiles(qt2std(selectedFiles));

	refreshImgView();
}

void MainWindow::on_addObjButton_clicked()
{
	// check wether dir/file/object have been selected
	QString filePath = currentFilePath();
	if (filePath.isEmpty())
		return;

	// add a new object to the our database
	Object *obj = annotations.newObject(filePath.toStdString());

	// refresh the TableWidget for the objects
	refreshObjView();

	// set the new object as current object
	setCurrentTableItem(obj->getID());
}

void MainWindow::on_delObjButton_clicked()
{
	// check wether dir/file/object have been selected
	ID id = currentObjectID();
	IDList idList = selectedObjectIDs();
	int iRow = objTableWidget->currentRow();
	if (id < 0 || idList.size() <= 0)
		return;

	// ask user for confirmation
	if (QMessageBox::Yes != QMessageBox::question(
			this,
			"Removing Objects from DB",
			"Are you sure that you would like to remove"
			"the selected objects from the image?",
			QMessageBox::Yes, QMessageBox::No))
		return;


	// remove selected objects from the file
	annotations.removeObjects(idList);

	// set the object before the last current object as current object
	if (iRow >= objTableWidget->rowCount())
		iRow = objTableWidget->rowCount() - 1;
	if (iRow >= 0)
		objTableWidget->setCurrentCell(iRow, -1);

	refreshObjView();
}

void MainWindow::on_addPropertyButton_clicked()
{
	// get the input from the lineEdit field
	QString newProperty = propertyLineEdit->text().trimmed().toLower().replace(QRegExp("\\s+|:"), "_");

	// check wether the property already exists or not
	if (propertyList.contains(newProperty) || reservedProperties.contains(newProperty) || newProperty.isEmpty())
		return;
	propertyList << newProperty;

	// clear the text from the lineEdit
	propertyLineEdit->setText("");

	refreshObjView();
}

//void MainWindow::on_objTypeComboBox_currentIndexChanged(const QString &text)
//{
//	// check wether dir/file/object have been selected
//	QString iFile = currentFile();
//	QString iDir = currentDir();
//	int iObj = currentObj();
//	if (iFile.isEmpty() || iDir.isEmpty() || iObj < 0)
//		return;
//
//	// set the new object type
//	IAObj *obj = annotations.getObj(iDir, iFile, iObj);
//	if (obj)
//		obj->type = text;
//
//	// update the text of the object which type has been changed
//	QListWidgetItem *item = objTableWidget->item(iObj);
//	if (item)
//		item->setText(text);
//
//	// save the last chosen obj type
//	lastObjType = text;
//}

void MainWindow::on_imgTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	// check wether dir/file/object have been selected
	QString filePath = currentFilePath();
	if (filePath.isEmpty())
		return;

	// check wether we have a relative or absolute path
	QString absoluteDir;
	if (filePath[0] != '/')
		filePath = lastDatabasePath + "/" + filePath;

	// load new file
	QPixmap tmpPixmap(filePath);
	pixmapWidget->setPixmap(tmpPixmap);

	// emit the signal that the file changed
	emit activeFileChanged(currentFilePath());

	// refresh the objTableWidget
	refreshObjView();

	// select the first object as current obj or no object .. refresh will be done
	// implicitely through the selection
	if (objTableWidget->rowCount() > 0)
		objTableWidget->setCurrentCell(0, 0);
	else {
		emit activeObjectChanged(-1);
		emit selectedObjectsChanged(IDList());
	}
}

void MainWindow::on_objTableWidget_currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)
{
	emit activeObjectChanged(currentObjectID());
}

void MainWindow::on_objTableWidget_itemSelectionChanged()
{
	IDList selectedObjs = selectedObjectIDs();
	if (selectedObjs.size() <= 1)
		emit selectedObjectsChanged(fileObjectIDs());
	else
		emit selectedObjectsChanged(selectedObjs);
}

void MainWindow::on_objTableWidget_itemChanged(QTableWidgetItem* item)
{
	// get the object in the data base
	ID objID = objectIDFromItem(item);
	Object *obj = annotations.getObject(objID);
	if (NULL == obj)
		return;

	// query the properties and save the in the database
	string property = propertyList[item->column()].toStdString();
	string value = item->text().toStdString();
	if (property.empty())
		obj->clear(property);
	else
		obj->set(property, value);

	objTableWidget->resizeColumnsToContents();
//	refreshObjView();
}

//void MainWindow::on_objListWidget_itemSelectionChanged()
//{
//	// check wether dir/file/object have been selected
//	QString iFile = currentFile();
//	QString iDir = currentDir();
//	int iObj = currentObj();
//	if (iFile.isEmpty() || iDir.isEmpty() || iObj < 0)
//		return;
//
//	// collect all selected tags
//	const QList<QListWidgetItem *> selectedTags = tagListWidget->selectedItems();
//	QStringList newTags;
//	QString tmpStr;
//	for (int i = 0; i < selectedTags.count(); i++) {
//		tmpStr = selectedTags[i]->text();
//		if (!tmpStr.isEmpty())
//			newTags << tmpStr;
//	}
//
//	// update the object's tag list
//	IAObj *obj = annotations.getObj(iDir, iFile, iObj);
//	if (obj) {
//		obj->tags.clear();
//		obj->tags << newTags;
//	}
//}

//void MainWindow::refreshTagView()
//{
//	// block all signals from the tagListWidget
//	tagListWidget->blockSignals(true);
//
//	// clear all tags and the tags from our internal tag list (containing all tags)
//	tagListWidget->clear();
//	tagListWidget->addItems(tagList);
//
//	// check wether dir/file/object have been selected
//	QString iFile = currentFile();
//	QString iDir = currentDir();
//	int iObj = currentObj();
//	if (!iFile.isEmpty() || !iDir.isEmpty() || iObj >= 0) {
//		// select the tags for the current obj
//		IAObj *obj = annotations.getObj(iDir, iFile, iObj);
//		if (obj)
//			for (int i = 0; i < obj->tags.count(); i++) {
//				// find the corresponding tag and select it
//				QString currentTag = obj->tags[i];
//				QList<QListWidgetItem *> foundItems = tagListWidget->findItems(currentTag, Qt::MatchExactly);
//				if (foundItems.count() != 1)
//					continue;
//
//				tagListWidget->setItemSelected(foundItems[0], true);
//			}
//	}
//
//	// unblock signals
//	tagListWidget->blockSignals(false);
//}

void MainWindow::refreshObjView()
{
	// check wether dir/file/object have been selected
	string filePath = currentFilePath().toStdString();
	if (filePath.empty()) {
		objTableWidget->clear();
		return;
	}

	// block the signals of the objTableWidget
	ID currentID = currentObjectID();
	objTableWidget->blockSignals(true);
	objTableWidget->setSortingEnabled(false);

	// adjust the size of the table
	int nRows = annotations.numOfObjects(ImgAnnotations::dirPath(filePath), ImgAnnotations::fileName(filePath));
	int nCols = propertyList.size();
	objTableWidget->clear();
	objTableWidget->setRowCount(nRows);
	objTableWidget->setColumnCount(nCols);

	// clear our QTableWidget and add the objects of the current image to it
	QRegExp expFloat("[e0-9\\.\\+\\-]+");
	QRegExp expInt("[0-9\\+\\-]+");
	QTableWidgetItem *newItem;
	int iRow = 0;
	BOOST_FOREACH(Object *obj, annotations.getObjects(ImgAnnotations::dirPath(filePath), ImgAnnotations::fileName(filePath))) {
		// we have to add each table cell manually
		int iCol = 0;

		foreach(QString qtProperty, propertyList) {
			string stdProperty = qtProperty.toStdString();
			newItem = new QTableWidgetItem;

			// set the property value as text
			if (0 == iCol) {
				newItem->setData(Qt::DisplayRole, QVariant(obj->getID()));
				newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
			}
			else {
				QString str = QString::fromStdString(obj->get(stdProperty));
				if (expInt.exactMatch(str)) {
					newItem->setData(Qt::DisplayRole, QVariant(str.toInt()));
					newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				}
				else if (expFloat.exactMatch(str)) {
					newItem->setData(Qt::DisplayRole, QVariant(str.toDouble()));
					newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				}
				else {
					newItem->setData(Qt::DisplayRole, QVariant(str));
					newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				}
			}

			// add the item
			objTableWidget->setItem(iRow, iCol, newItem);

			// increase the counter
			iCol++;
		}

		// increase the counter
		iRow++;
	}

	// set the labels for the header row/column
	objTableWidget->setHorizontalHeaderLabels(propertyList);
	objTableWidget->resizeColumnsToContents();
//	objTableWidget->resizeRowsToContents();
//	setCurrentTableItem(currentID);

	// unblock the signals
	objTableWidget->setSortingEnabled(true);
	objTableWidget->blockSignals(false);
}

void MainWindow::refreshImgView()
{
	// clear all items
	imgTreeWidget->blockSignals(true);
	imgTreeWidget->setUpdatesEnabled(false);
	imgTreeWidget->clear();

	// add all dirs and files to the QTreeWidget
	QList<QTreeWidgetItem*> newItems;
	BOOST_FOREACH(Dir *dir, annotations.getDirs()) {
		// construct a new directory entry
		QTreeWidgetItem *dirItem = new QTreeWidgetItem();
		imgTreeWidget->setItemExpanded(dirItem, true);
		dirItem->setText(0, QString::fromStdString(dir->getDirPath()));

		BOOST_FOREACH(string fileName, dir->getFileNames()) {
			// construct a new file entry
			QTreeWidgetItem *fileEntry = new QTreeWidgetItem(dirItem);
			fileEntry->setText(0, QString::fromStdString(fileName));
		}
		
		// collect item in list
		newItems.append(dirItem);
	}

	// add all items at once to the tree widget
	imgTreeWidget->addTopLevelItems(newItems);
	
	// sort the files + directories
	imgTreeWidget->sortItems(0, Qt::AscendingOrder);

	// unblock signals
	imgTreeWidget->setUpdatesEnabled(true);
	imgTreeWidget->blockSignals(false);
}

IDList MainWindow::selectedObjectIDs() const
{
	// collect the selected objects
	IDList idList;
	for (int i = 0; i < objTableWidget->rowCount(); i++) {
		QTableWidgetItem *item = objTableWidget->item(i, 0);
		if (item->isSelected())
			// add the object index to the list, denoting that this index
			// is to be removed from the list
			idList.push_back(static_cast<ID>(objectIDFromItem(item)));
	}
	return idList;
}

//IDList MainWindow::selectedObjectIDs() const
//{
//	IDList idList;
//
//	string filePath = currentFilePath().toStdString();
//	// get all objects associated with the current image
//	if (!filePath.empty())
//		idList = annotations.getObjectIDs(ImgAnnotations::dirPath(filePath), ImgAnnotations::fileName(filePath));
//
//	return idList;
//}

IDList MainWindow::fileObjectIDs() const
{
	IDList idList;

	string filePath = currentFilePath().toStdString();
	// get all objects associated with the current image
	if (!filePath.empty())
		idList = annotations.getObjectIDs(ImgAnnotations::dirPath(filePath), ImgAnnotations::fileName(filePath));

	return idList;
}

void MainWindow::nextPreviousFile(MainWindow::Direction direction)
{
	// choose the current items from the imgTreeWidget
	QTreeWidgetItem *current = imgTreeWidget->currentItem();
	if (!current)
		return;
	QTreeWidgetItem *currentParent = current->parent();

	if (!currentParent) {
		// we have a directory selected .. take the first file as current item
		current = current->child(0);
		currentParent = current->parent();
	}

	if (!current || !currentParent)
		return;

	// get the indeces
	int iParent = imgTreeWidget->indexOfTopLevelItem(currentParent);
	int iCurrent = currentParent->indexOfChild(current);

	// select the next file index
	if (direction == Up)
		iCurrent--;
	else
		iCurrent++;

	// the index may be negative .. in that case we switch the parent as well
	if (iCurrent < 0) {
		if (iParent > 0) {
			// get the directory before
			iParent--;
			currentParent = imgTreeWidget->topLevelItem(iParent);

			if (!currentParent)
				return;

			// get the last item from the directory before
			iCurrent = currentParent->childCount() - 1;
		}
		else
			// we are at the beginning ..
			iCurrent = 0;
	}
	// the index might be too large .. in that case we switch the parent as well
	else if (iCurrent >= currentParent->childCount()) {
		if (iParent < imgTreeWidget->topLevelItemCount() - 1) {
			// get the next directory
			iParent++;
			currentParent = imgTreeWidget->topLevelItem(iParent);

			if (!currentParent)
				return;

			// get the first item from the next directory
			iCurrent = 0;
		}
		else
			// we are at the end ..
			iCurrent = currentParent->childCount() - 1;
	}

	if (!currentParent)
		return;

	// we handled all special cases thus we may try to set the next current item
	current = currentParent->child(iCurrent);
	if (current)
		imgTreeWidget->setCurrentItem(current);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	// ask user for confirmation
	if (QMessageBox::Yes == QMessageBox::question(
			this,
			"Existing Application",
			"Are you sure to exist .. you might lose not saved data?",
			QMessageBox::Yes, QMessageBox::No))
		event->accept();
	else
		event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift) {
		event->accept();
		pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Browsing);
		statusBar()->showMessage("Browsing mode: select a bounding box visually");
	}
	else if (event->key() == Qt::Key_Control) {
		event->accept();
		pixmapWidget->setMouseMode(AnnotationsPixmapWidget::CreateDeleteFixPoint);
		statusBar()->showMessage("FixPoint mode: create or delete fix points, zoom in the image with the mouse wheel");
	}
	else
		event->ignore();
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift) {
		event->accept();
		pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
		statusBar()->clearMessage();
	}
	if (event->key() == Qt::Key_Control) {
		event->accept();
		pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);

		if (statusBar()->currentMessage().toLower().startsWith("fixpoint"))
			statusBar()->clearMessage();
	}
	else
		event->ignore();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
	if (!event->isAccepted()) {
		// see what to do with the event
		if (pixmapWidget->getMouseMode() == AnnotationsPixmapWidget::Normal) {
			// select a different file
			if (event->delta() < 0)
				nextPreviousFile(Down);
			else if (event->delta() > 0)
				nextPreviousFile(Up);
		}
		else {
			// forward the wheelEvent to the zoomSpinBox
			if (event->delta() < 0)
				zoomSpinBox->stepDown();
			else if (event->delta() > 0)
				zoomSpinBox->stepUp();
		}
		event->accept();
	}
}


