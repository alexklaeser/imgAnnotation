/********************************************************************************
** Form generated from reading ui file 'MainWindow.ui'
**
** Created: Thu Jan 17 15:05:51 2008
**      by: Qt User Interface Compiler version 4.3.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_MainWindow
{
public:
    QAction *actionOpenDatabase;
    QAction *actionSaveDatabase;
    QAction *actionQuit;
    QAction *actionSaveDatabaseAs;
    QAction *actionShortcutHelp;
    QAction *actionCopyObj;
    QAction *actionPasteObj;
    QAction *actionExtractImages;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuHelp;
    QMenu *menuMenu;
    QMenu *menuEdit;
    QStatusBar *statusbar;
    QDockWidget *objDockWidget;
    QWidget *objDockWidgetContents;
    QVBoxLayout *vboxLayout;
    QTableWidget *objTableWidget;
    QHBoxLayout *hboxLayout;
    QLineEdit *propertyLineEdit;
    QPushButton *addPropertyButton;
    QSpacerItem *spacerItem;
    QPushButton *addObjButton;
    QPushButton *delObjButton;
    QDockWidget *imgDockWidget;
    QWidget *imgDockWidgetContents;
    QVBoxLayout *vboxLayout1;
    QTreeWidget *imgTreeWidget;
    QHBoxLayout *hboxLayout1;
    QPushButton *addImgButton;
    QPushButton *delImgButton;
    QHBoxLayout *hboxLayout2;
    QLabel *zoomLabel;
    QDoubleSpinBox *zoomSpinBox;
    QSpacerItem *spacerItem1;

    void setupUi(QMainWindow *MainWindow)
    {
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(945, 844);
    actionOpenDatabase = new QAction(MainWindow);
    actionOpenDatabase->setObjectName(QString::fromUtf8("actionOpenDatabase"));
    actionSaveDatabase = new QAction(MainWindow);
    actionSaveDatabase->setObjectName(QString::fromUtf8("actionSaveDatabase"));
    actionQuit = new QAction(MainWindow);
    actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
    actionSaveDatabaseAs = new QAction(MainWindow);
    actionSaveDatabaseAs->setObjectName(QString::fromUtf8("actionSaveDatabaseAs"));
    actionShortcutHelp = new QAction(MainWindow);
    actionShortcutHelp->setObjectName(QString::fromUtf8("actionShortcutHelp"));
    actionCopyObj = new QAction(MainWindow);
    actionCopyObj->setObjectName(QString::fromUtf8("actionCopyObj"));
    actionCopyObj->setEnabled(true);
    actionPasteObj = new QAction(MainWindow);
    actionPasteObj->setObjectName(QString::fromUtf8("actionPasteObj"));
    actionPasteObj->setEnabled(true);
    actionExtractImages = new QAction(MainWindow);
    actionExtractImages->setObjectName(QString::fromUtf8("actionExtractImages"));
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    MainWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(MainWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 945, 29));
    menuHelp = new QMenu(menubar);
    menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
    menuMenu = new QMenu(menubar);
    menuMenu->setObjectName(QString::fromUtf8("menuMenu"));
    menuEdit = new QMenu(menubar);
    menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
    MainWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);
    objDockWidget = new QDockWidget(MainWindow);
    objDockWidget->setObjectName(QString::fromUtf8("objDockWidget"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(objDockWidget->sizePolicy().hasHeightForWidth());
    objDockWidget->setSizePolicy(sizePolicy);
    objDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
    objDockWidgetContents = new QWidget(objDockWidget);
    objDockWidgetContents->setObjectName(QString::fromUtf8("objDockWidgetContents"));
    vboxLayout = new QVBoxLayout(objDockWidgetContents);
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout->setMargin(9);
#endif
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    objTableWidget = new QTableWidget(objDockWidgetContents);
    objTableWidget->setObjectName(QString::fromUtf8("objTableWidget"));
    objTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    objTableWidget->setAlternatingRowColors(true);
    objTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    objTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    objTableWidget->setSortingEnabled(true);

    vboxLayout->addWidget(objTableWidget);

    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    propertyLineEdit = new QLineEdit(objDockWidgetContents);
    propertyLineEdit->setObjectName(QString::fromUtf8("propertyLineEdit"));

    hboxLayout->addWidget(propertyLineEdit);

    addPropertyButton = new QPushButton(objDockWidgetContents);
    addPropertyButton->setObjectName(QString::fromUtf8("addPropertyButton"));

    hboxLayout->addWidget(addPropertyButton);

    spacerItem = new QSpacerItem(131, 27, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    addObjButton = new QPushButton(objDockWidgetContents);
    addObjButton->setObjectName(QString::fromUtf8("addObjButton"));

    hboxLayout->addWidget(addObjButton);

    delObjButton = new QPushButton(objDockWidgetContents);
    delObjButton->setObjectName(QString::fromUtf8("delObjButton"));

    hboxLayout->addWidget(delObjButton);


    vboxLayout->addLayout(hboxLayout);

    objDockWidget->setWidget(objDockWidgetContents);
    MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), objDockWidget);
    imgDockWidget = new QDockWidget(MainWindow);
    imgDockWidget->setObjectName(QString::fromUtf8("imgDockWidget"));
    sizePolicy.setHeightForWidth(imgDockWidget->sizePolicy().hasHeightForWidth());
    imgDockWidget->setSizePolicy(sizePolicy);
    imgDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
    imgDockWidgetContents = new QWidget(imgDockWidget);
    imgDockWidgetContents->setObjectName(QString::fromUtf8("imgDockWidgetContents"));
    vboxLayout1 = new QVBoxLayout(imgDockWidgetContents);
#ifndef Q_OS_MAC
    vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout1->setMargin(9);
#endif
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    vboxLayout1->setContentsMargins(0, 0, 0, 0);
    imgTreeWidget = new QTreeWidget(imgDockWidgetContents);
    imgTreeWidget->setObjectName(QString::fromUtf8("imgTreeWidget"));
    imgTreeWidget->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::NoEditTriggers);
    imgTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    imgTreeWidget->setIndentation(20);
    imgTreeWidget->setUniformRowHeights(true);
    imgTreeWidget->setSortingEnabled(true);

    vboxLayout1->addWidget(imgTreeWidget);

    hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    addImgButton = new QPushButton(imgDockWidgetContents);
    addImgButton->setObjectName(QString::fromUtf8("addImgButton"));

    hboxLayout1->addWidget(addImgButton);

    delImgButton = new QPushButton(imgDockWidgetContents);
    delImgButton->setObjectName(QString::fromUtf8("delImgButton"));

    hboxLayout1->addWidget(delImgButton);


    vboxLayout1->addLayout(hboxLayout1);

    hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout2->setSpacing(6);
#endif
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    zoomLabel = new QLabel(imgDockWidgetContents);
    zoomLabel->setObjectName(QString::fromUtf8("zoomLabel"));

    hboxLayout2->addWidget(zoomLabel);

    zoomSpinBox = new QDoubleSpinBox(imgDockWidgetContents);
    zoomSpinBox->setObjectName(QString::fromUtf8("zoomSpinBox"));
    zoomSpinBox->setMaximum(19.99);
    zoomSpinBox->setMinimum(0.01);
    zoomSpinBox->setSingleStep(0.25);
    zoomSpinBox->setValue(1);

    hboxLayout2->addWidget(zoomSpinBox);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);


    vboxLayout1->addLayout(hboxLayout2);

    imgDockWidget->setWidget(imgDockWidgetContents);
    MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), imgDockWidget);

    menubar->addAction(menuMenu->menuAction());
    menubar->addAction(menuEdit->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menuHelp->addAction(actionShortcutHelp);
    menuMenu->addAction(actionOpenDatabase);
    menuMenu->addAction(actionSaveDatabase);
    menuMenu->addAction(actionSaveDatabaseAs);
    menuMenu->addSeparator();
    menuMenu->addAction(actionExtractImages);
    menuMenu->addSeparator();
    menuMenu->addAction(actionQuit);
    menuEdit->addAction(actionCopyObj);
    menuEdit->addAction(actionPasteObj);

    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ImageAnnotation", 0, QApplication::UnicodeUTF8));
    actionOpenDatabase->setText(QApplication::translate("MainWindow", "&Open Database", 0, QApplication::UnicodeUTF8));
    actionOpenDatabase->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
    actionSaveDatabase->setText(QApplication::translate("MainWindow", "&Save Database", 0, QApplication::UnicodeUTF8));
    actionSaveDatabase->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
    actionQuit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
    actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
    actionSaveDatabaseAs->setText(QApplication::translate("MainWindow", "Save Database &As ...", 0, QApplication::UnicodeUTF8));
    actionSaveDatabaseAs->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
    actionShortcutHelp->setText(QApplication::translate("MainWindow", "&Shortcuts", 0, QApplication::UnicodeUTF8));
    actionCopyObj->setText(QApplication::translate("MainWindow", "&Copy Object", 0, QApplication::UnicodeUTF8));
    actionCopyObj->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
    actionPasteObj->setText(QApplication::translate("MainWindow", "&Paste Object", 0, QApplication::UnicodeUTF8));
    actionPasteObj->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0, QApplication::UnicodeUTF8));
    actionExtractImages->setText(QApplication::translate("MainWindow", "&Extract Images", 0, QApplication::UnicodeUTF8));
    actionExtractImages->setShortcut(QApplication::translate("MainWindow", "Ctrl+E", 0, QApplication::UnicodeUTF8));
    menuHelp->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
    menuMenu->setTitle(QApplication::translate("MainWindow", "&Database", 0, QApplication::UnicodeUTF8));
    menuEdit->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
    objDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Objects in Image", 0, QApplication::UnicodeUTF8));
    addPropertyButton->setText(QApplication::translate("MainWindow", "Add Property", 0, QApplication::UnicodeUTF8));
    addObjButton->setText(QApplication::translate("MainWindow", "Add Object", 0, QApplication::UnicodeUTF8));
    addObjButton->setShortcut(QApplication::translate("MainWindow", "Ins", 0, QApplication::UnicodeUTF8));
    delObjButton->setText(QApplication::translate("MainWindow", "Delete Objects", 0, QApplication::UnicodeUTF8));
    delObjButton->setShortcut(QApplication::translate("MainWindow", "Del", 0, QApplication::UnicodeUTF8));
    imgDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Images in Data Base", 0, QApplication::UnicodeUTF8));
    imgTreeWidget->headerItem()->setText(0, QApplication::translate("MainWindow", "File/Directory", 0, QApplication::UnicodeUTF8));
    addImgButton->setText(QApplication::translate("MainWindow", "Add File", 0, QApplication::UnicodeUTF8));
    delImgButton->setText(QApplication::translate("MainWindow", "Delete Files", 0, QApplication::UnicodeUTF8));
    zoomLabel->setText(QApplication::translate("MainWindow", "Zoom (in %):", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_MAINWINDOW_H
