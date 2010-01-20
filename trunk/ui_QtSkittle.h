/********************************************************************************
** Form generated from reading ui file 'QtSkittle.ui'
**
** Created: Fri Apr 17 10:15:41 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTSKITTLE_H
#define UI_QTSKITTLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_SkittleGUI
{
public:
    QAction *actionNucleotide_Display;
    QAction *actionFrequency_Map;
    QAction *actionSkittle_Website;
    QAction *actionReset_View;
    QAction *actionOpen;
    QAction *actionExit;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QToolButton *moveButton;
    QToolButton *selectButton;
    QToolButton *resizeButton;
    QToolButton *toolButton_5;
    QToolButton *toolButton_6;
    QToolButton *toolButton_7;
    GLWidget *glWidget;
    QGroupBox *zoomBox;
    QSlider *zoomSlider;
    QLabel *label;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QGroupBox *groupBox_2;
    QSpinBox *sizeDial;
    QGroupBox *groupBox_3;
    QSpinBox *widthDial;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QGroupBox *groupBox_4;
    QSpinBox *startDial;
    QGroupBox *zoomBox_2;
    QSpinBox *zoomDial;
    QCheckBox *textureCheckBox;
    QCheckBox *invertCheckBox;
    QTextBrowser *textBrowser;
    QScrollBar *verticalScrollBar;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuWindows;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SkittleGUI)
    {
    if (SkittleGUI->objectName().isEmpty())
        SkittleGUI->setObjectName(QString::fromUtf8("SkittleGUI"));
    SkittleGUI->resize(936, 610);
    actionNucleotide_Display = new QAction(SkittleGUI);
    actionNucleotide_Display->setObjectName(QString::fromUtf8("actionNucleotide_Display"));
    actionFrequency_Map = new QAction(SkittleGUI);
    actionFrequency_Map->setObjectName(QString::fromUtf8("actionFrequency_Map"));
    actionSkittle_Website = new QAction(SkittleGUI);
    actionSkittle_Website->setObjectName(QString::fromUtf8("actionSkittle_Website"));
    actionReset_View = new QAction(SkittleGUI);
    actionReset_View->setObjectName(QString::fromUtf8("actionReset_View"));
    actionOpen = new QAction(SkittleGUI);
    actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    actionExit = new QAction(SkittleGUI);
    actionExit->setObjectName(QString::fromUtf8("actionExit"));
    centralwidget = new QWidget(SkittleGUI);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    centralwidget->setSizeIncrement(QSize(100, 100));
    gridLayout = new QGridLayout(centralwidget);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    groupBox = new QGroupBox(centralwidget);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setMinimumSize(QSize(80, 400));
    groupBox->setMaximumSize(QSize(80, 16777215));
    moveButton = new QToolButton(groupBox);
    moveButton->setObjectName(QString::fromUtf8("moveButton"));
    moveButton->setGeometry(QRect(18, 45, 41, 20));
    selectButton = new QToolButton(groupBox);
    selectButton->setObjectName(QString::fromUtf8("selectButton"));
    selectButton->setGeometry(QRect(18, 145, 41, 20));
    resizeButton = new QToolButton(groupBox);
    resizeButton->setObjectName(QString::fromUtf8("resizeButton"));
    resizeButton->setGeometry(QRect(18, 95, 44, 20));
    toolButton_5 = new QToolButton(groupBox);
    toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));
    toolButton_5->setGeometry(QRect(18, 195, 25, 20));
    toolButton_6 = new QToolButton(groupBox);
    toolButton_6->setObjectName(QString::fromUtf8("toolButton_6"));
    toolButton_6->setGeometry(QRect(18, 245, 25, 20));
    toolButton_7 = new QToolButton(groupBox);
    toolButton_7->setObjectName(QString::fromUtf8("toolButton_7"));
    toolButton_7->setGeometry(QRect(18, 295, 25, 20));

    gridLayout->addWidget(groupBox, 0, 0, 1, 1);

    glWidget = new GLWidget(centralwidget);
    glWidget->setObjectName(QString::fromUtf8("glWidget"));
    glWidget->setEnabled(true);
    glWidget->setMinimumSize(QSize(400, 400));
    glWidget->setSizeIncrement(QSize(0, 0));
    glWidget->setCursor(QCursor(Qt::SizeAllCursor));
    glWidget->setMouseTracking(true);
    glWidget->setFocusPolicy(Qt::ClickFocus);
    glWidget->setAutoFillBackground(false);

    gridLayout->addWidget(glWidget, 0, 1, 1, 2);

    zoomBox = new QGroupBox(centralwidget);
    zoomBox->setObjectName(QString::fromUtf8("zoomBox"));
    zoomBox->setMinimumSize(QSize(140, 70));
    zoomBox->setMaximumSize(QSize(16777215, 70));
    zoomSlider = new QSlider(zoomBox);
    zoomSlider->setObjectName(QString::fromUtf8("zoomSlider"));
    zoomSlider->setGeometry(QRect(10, 50, 512, 21));
    zoomSlider->setMinimum(1);
    zoomSlider->setMaximum(200);
    zoomSlider->setValue(100);
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setInvertedAppearance(false);
    zoomSlider->setTickPosition(QSlider::TicksAbove);
    zoomSlider->setTickInterval(5);
    label = new QLabel(zoomBox);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 20, 501, 20));

    gridLayout->addWidget(zoomBox, 1, 0, 1, 3);

    scrollArea = new QScrollArea(centralwidget);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setMinimumSize(QSize(120, 0));
    scrollArea->setMaximumSize(QSize(120, 16777215));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 118, 474));
    pushButton = new QPushButton(scrollAreaWidgetContents);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setGeometry(QRect(10, 40, 91, 23));
    pushButton_2 = new QPushButton(scrollAreaWidgetContents);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton_2->setGeometry(QRect(10, 80, 91, 23));
    groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 433, 101, 41));
    sizeDial = new QSpinBox(groupBox_2);
    sizeDial->setObjectName(QString::fromUtf8("sizeDial"));
    sizeDial->setGeometry(QRect(11, 15, 81, 20));
    QFont font;
    font.setPointSize(14);
    sizeDial->setFont(font);
    sizeDial->setMinimum(1);
    sizeDial->setMaximum(100000);
    sizeDial->setSingleStep(1000);
    sizeDial->setValue(40000);
    groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    groupBox_3->setGeometry(QRect(10, 390, 107, 41));
    widthDial = new QSpinBox(groupBox_3);
    widthDial->setObjectName(QString::fromUtf8("widthDial"));
    widthDial->setGeometry(QRect(2, 15, 81, 20));
    widthDial->setFont(font);
    widthDial->setMinimum(1);
    widthDial->setMaximum(40000);
    widthDial->setValue(200);
    pushButton_3 = new QPushButton(groupBox_3);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton_3->setGeometry(QRect(86, 9, 18, 16));
    pushButton_4 = new QPushButton(groupBox_3);
    pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
    pushButton_4->setGeometry(QRect(86, 25, 18, 16));
    groupBox_4 = new QGroupBox(scrollAreaWidgetContents);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    groupBox_4->setGeometry(QRect(10, 348, 101, 41));
    startDial = new QSpinBox(groupBox_4);
    startDial->setObjectName(QString::fromUtf8("startDial"));
    startDial->setGeometry(QRect(3, 15, 81, 20));
    startDial->setFont(font);
    startDial->setMinimum(1);
    startDial->setMaximum(400000000);
    startDial->setValue(1);
    zoomBox_2 = new QGroupBox(scrollAreaWidgetContents);
    zoomBox_2->setObjectName(QString::fromUtf8("zoomBox_2"));
    zoomBox_2->setGeometry(QRect(10, 477, 101, 41));
    zoomDial = new QSpinBox(zoomBox_2);
    zoomDial->setObjectName(QString::fromUtf8("zoomDial"));
    zoomDial->setGeometry(QRect(11, 15, 81, 20));
    zoomDial->setFont(font);
    zoomDial->setMinimum(1);
    zoomDial->setMaximum(100000);
    zoomDial->setSingleStep(1000);
    zoomDial->setValue(100);
    textureCheckBox = new QCheckBox(scrollAreaWidgetContents);
    textureCheckBox->setObjectName(QString::fromUtf8("textureCheckBox"));
    textureCheckBox->setGeometry(QRect(6, 318, 104, 18));
    textureCheckBox->setChecked(true);
    invertCheckBox = new QCheckBox(scrollAreaWidgetContents);
    invertCheckBox->setObjectName(QString::fromUtf8("invertCheckBox"));
    invertCheckBox->setGeometry(QRect(8, 294, 110, 18));
    scrollArea->setWidget(scrollAreaWidgetContents);

    gridLayout->addWidget(scrollArea, 0, 4, 2, 1);

    textBrowser = new QTextBrowser(centralwidget);
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

    gridLayout->addWidget(textBrowser, 2, 0, 1, 5);

    verticalScrollBar = new QScrollBar(centralwidget);
    verticalScrollBar->setObjectName(QString::fromUtf8("verticalScrollBar"));
    verticalScrollBar->setMinimumSize(QSize(7, 20));
    verticalScrollBar->setMaximum(10000);
    verticalScrollBar->setSingleStep(100);
    verticalScrollBar->setPageStep(1000);
    verticalScrollBar->setOrientation(Qt::Vertical);

    gridLayout->addWidget(verticalScrollBar, 0, 3, 1, 1);

    SkittleGUI->setCentralWidget(centralwidget);
    menubar = new QMenuBar(SkittleGUI);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 936, 21));
    menuFile = new QMenu(menubar);
    menuFile->setObjectName(QString::fromUtf8("menuFile"));
    menuView = new QMenu(menubar);
    menuView->setObjectName(QString::fromUtf8("menuView"));
    menuWindows = new QMenu(menubar);
    menuWindows->setObjectName(QString::fromUtf8("menuWindows"));
    menuHelp = new QMenu(menubar);
    menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
    SkittleGUI->setMenuBar(menubar);
    statusbar = new QStatusBar(SkittleGUI);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    SkittleGUI->setStatusBar(statusbar);

    menubar->addAction(menuFile->menuAction());
    menubar->addAction(menuView->menuAction());
    menubar->addAction(menuWindows->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionExit);
    menuView->addAction(actionReset_View);
    menuWindows->addAction(actionNucleotide_Display);
    menuWindows->addAction(actionFrequency_Map);
    menuHelp->addAction(actionSkittle_Website);

    retranslateUi(SkittleGUI);
    QObject::connect(sizeDial, SIGNAL(valueChanged(int)), glWidget, SLOT(changeLength(int)));
    QObject::connect(widthDial, SIGNAL(valueChanged(int)), glWidget, SLOT(changeWidth(int)));
    QObject::connect(startDial, SIGNAL(valueChanged(int)), glWidget, SLOT(changeStart(int)));
    QObject::connect(glWidget, SIGNAL(startChanged(int)), startDial, SLOT(setValue(int)));
    QObject::connect(glWidget, SIGNAL(widthChanged(int)), widthDial, SLOT(setValue(int)));
    QObject::connect(glWidget, SIGNAL(lengthChanged(int)), sizeDial, SLOT(setValue(int)));

    QMetaObject::connectSlotsByName(SkittleGUI);
    } // setupUi

    void retranslateUi(QMainWindow *SkittleGUI)
    {
    SkittleGUI->setWindowTitle(QApplication::translate("SkittleGUI", "Skittle", 0, QApplication::UnicodeUTF8));
    actionNucleotide_Display->setText(QApplication::translate("SkittleGUI", "Nucleotide Display", 0, QApplication::UnicodeUTF8));
    actionFrequency_Map->setText(QApplication::translate("SkittleGUI", "Frequency Map", 0, QApplication::UnicodeUTF8));
    actionSkittle_Website->setText(QApplication::translate("SkittleGUI", "Skittle Website", 0, QApplication::UnicodeUTF8));
    actionReset_View->setText(QApplication::translate("SkittleGUI", "Reset View", 0, QApplication::UnicodeUTF8));
    actionOpen->setText(QApplication::translate("SkittleGUI", "Open...", 0, QApplication::UnicodeUTF8));
    actionExit->setText(QApplication::translate("SkittleGUI", "Exit", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("SkittleGUI", "Tools", 0, QApplication::UnicodeUTF8));
    moveButton->setText(QApplication::translate("SkittleGUI", "Move", 0, QApplication::UnicodeUTF8));
    selectButton->setText(QApplication::translate("SkittleGUI", "Select", 0, QApplication::UnicodeUTF8));
    resizeButton->setText(QApplication::translate("SkittleGUI", "Resize", 0, QApplication::UnicodeUTF8));
    toolButton_5->setText(QApplication::translate("SkittleGUI", "...", 0, QApplication::UnicodeUTF8));
    toolButton_6->setText(QApplication::translate("SkittleGUI", "...", 0, QApplication::UnicodeUTF8));
    toolButton_7->setText(QApplication::translate("SkittleGUI", "...", 0, QApplication::UnicodeUTF8));
    zoomBox->setTitle(QApplication::translate("SkittleGUI", "Zoom", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("SkittleGUI", "1                                                                             100                                                                           200", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("SkittleGUI", "Nucleotide", 0, QApplication::UnicodeUTF8));
    pushButton_2->setText(QApplication::translate("SkittleGUI", "Frequency Map", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("SkittleGUI", "Display Length", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_ACCESSIBILITY
    sizeDial->setAccessibleName(QApplication::translate("SkittleGUI", "Display Width", "Width", QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY

    sizeDial->setSuffix(QString());
    groupBox_3->setTitle(QApplication::translate("SkittleGUI", "Display Width", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_ACCESSIBILITY
    widthDial->setAccessibleName(QApplication::translate("SkittleGUI", "Display Width", "Width", QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY

    widthDial->setSuffix(QString());
    pushButton_3->setText(QApplication::translate("SkittleGUI", "+", 0, QApplication::UnicodeUTF8));
    pushButton_4->setText(QApplication::translate("SkittleGUI", "-", 0, QApplication::UnicodeUTF8));
    groupBox_4->setTitle(QApplication::translate("SkittleGUI", "Start Position", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_ACCESSIBILITY
    startDial->setAccessibleName(QApplication::translate("SkittleGUI", "Display Width", "Width", QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY

    startDial->setSuffix(QString());
    zoomBox_2->setTitle(QApplication::translate("SkittleGUI", "Zoom", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_ACCESSIBILITY
    zoomDial->setAccessibleName(QApplication::translate("SkittleGUI", "Display Width", "Width", QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY

    zoomDial->setSuffix(QString());
    textureCheckBox->setText(QApplication::translate("SkittleGUI", "Texture Optimization", 0, QApplication::UnicodeUTF8));
    invertCheckBox->setText(QApplication::translate("SkittleGUI", "Minus Strand", 0, QApplication::UnicodeUTF8));
    menuFile->setTitle(QApplication::translate("SkittleGUI", "File", 0, QApplication::UnicodeUTF8));
    menuView->setTitle(QApplication::translate("SkittleGUI", "View", 0, QApplication::UnicodeUTF8));
    menuWindows->setTitle(QApplication::translate("SkittleGUI", "Windows", 0, QApplication::UnicodeUTF8));
    menuHelp->setTitle(QApplication::translate("SkittleGUI", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SkittleGUI: public Ui_SkittleGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSKITTLE_H
