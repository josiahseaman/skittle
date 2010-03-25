#include <QtGui>
#include <QScrollArea>
#include <QtGui/QScrollBar>

#include <iostream>
#include <string>

#include "MainWindow.h"
#include "FastaReader.h"
#include "GtfReader.h"
#include "glwidget.h"
#include "AbstractGraph.h"
#include "NucleotideDisplay.h"
#include "FrequencyMap.h"
#include "AnnotationDisplay.h"
#include "CylinderDisplay.h"
#include "AlignmentDisplay.h"
#include "OligomerDisplay.h"
#include "HighlightDisplay.h"
#include "ViewManager.h"

MainWindow::MainWindow()
{
	scrollArea = new QFrame;
	scrollArea->setBackgroundRole(QPalette::Dark);
	
	setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
	createActions();
	createMenus();
	createToolbars();
	createDocks();
	createStatusBar();
	oldScale = 1;
	
	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );

	viewManager	= new ViewManager(getDisplayVariables());
	glWidget = new GLWidget(getDisplayVariables(), this);
	glWidget2 = new GLWidget(getDisplayVariables(), this);
	viewManager->changeSelection(glWidget);
	
	QFrame* subFrame = new QFrame;
	
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(glWidget);
	hLayout->addWidget(glWidget2);
	hLayout->addWidget(verticalScrollBar);
	subFrame->setLayout(hLayout);
	
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(subFrame);
	vLayout->addWidget(horizontalScrollBar);
	scrollArea->setLayout(vLayout);
	
	setCentralWidget(scrollArea);
  

	createConnections();
	glWidget->createButtons();
	glWidget2->createButtons();
	readSettings();
}

void MainWindow::addDisplayActions(AbstractGraph* display)
{
	if(display->actionLabel.size() > 0)
	{
		QAction* presetAction = new QAction(QString(display->actionLabel.c_str()),this);
		presetAction->setStatusTip(QString(display->actionTooltip.c_str()));
		presetAction->setData(QString(display->actionData.c_str()));
		connect(presetAction,SIGNAL(triggered()),display, SLOT(toggleVisibility()));
	
		presetMenu->addAction(presetAction);
		presetToolBar->addAction(presetAction);
	}
	else
	{
		glWidget->print("Tried to add display mode with no label, aborting...");
	}
}
void MainWindow::addDisplayDivider()
{
	presetToolBar->addSeparator();	
}

void MainWindow::createActions()
{
	moveAction = new QAction("Move",this);	
	resizeAction = new QAction("Resize",this);	
	zoomAction = new QAction("Zoom",this);	
	selectAction = new QAction("Select",this);	
	findAction = new QAction("&Find",this);	
	addAnnotationAction = new QAction("Add Annotation",this);	
	nextAnnotationAction = new QAction("Next Annotation",this);	
	prevAnnotationAction = new QAction("Previous Annotation",this);	
	browseCommunityAction = new QAction("Browse Community Research",this);	
	delAnnotationAction = new QAction("Delete Current Bookmark",this);	
	
	/*****TODO: NOT CURRENTLY IN USE ********/
	findSequenceAction = new QAction("Find Sequence",this);	
	findSequenceAction->setStatusTip("Find Arbitrary Sequence");
	findNextAction = new QAction("Find Next", this);
	findNextAction->setStatusTip("Jump to Next Instance of Current Sequence");
	findPrevAction = new QAction("Find Previous", this);
	findPrevAction->setStatusTip("Jump to Previous Instance of Current Sequence");
	hilightResultsAction = new QAction("Highlight Results",this);
	hilightResultsAction->setStatusTip("Highlight All copies of Current Sequence");
	hilightResultsAction->setCheckable(true);
	
	addViewAction = new QAction("Add New View Panel",this);	
	
	openAction = new QAction("&Open",this);
	openAction->setStatusTip("Open a Sequence File");
	
	importAction = new QAction("&Import GTF Annotation File",this);
	importAction->setStatusTip("Open GTF / GFF Annotation File");
	
	exitAction = new QAction("E&xit",this);
	exitAction->setStatusTip("Close Program");
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu("&File");
	fileMenu->addAction(openAction);
	fileMenu->addSeparator();
	fileMenu->addAction(importAction);
	fileMenu->addAction(exitAction);
	searchMenu = menuBar()->addMenu("&Search");
	searchMenu->addAction(findSequenceAction);
	searchMenu->addAction(findNextAction);
	searchMenu->addAction(findPrevAction);
	searchMenu->addAction(hilightResultsAction);
	viewMenu = menuBar()->addMenu("&View");
	toolBarMenu = viewMenu->addMenu("ToolBar");
	presetMenu = viewMenu->addMenu("Presets");
	viewMenu->addSeparator();
	viewMenu->addAction(addViewAction);
	
	annotationMenu = menuBar()->addMenu("&Annotations");
	annotationMenu->addAction(addAnnotationAction);
	annotationMenu->addAction(nextAnnotationAction);
	annotationMenu->addAction(prevAnnotationAction);
	annotationMenu->addAction(browseCommunityAction);
	annotationMenu->addAction(delAnnotationAction);
	toolMenu = menuBar()->addMenu("&Tools");
	toolMenu->addAction(moveAction);
	toolMenu->addAction(resizeAction);
	toolMenu->addAction(zoomAction);
	toolMenu->addAction(selectAction);
	toolMenu->addAction(findAction);
	toolActionGroup = new QActionGroup(this);
	toolActionGroup->addAction(moveAction);
	toolActionGroup->addAction(resizeAction);
	toolActionGroup->addAction(zoomAction);
	toolActionGroup->addAction(selectAction);
	toolActionGroup->addAction(findAction);
	
}

void MainWindow::createToolbars()
{
	annotationToolBar = addToolBar("Annotations");
	annotationToolBar->setObjectName("annotations");
	annotationToolBar->addAction(addAnnotationAction);
	annotationToolBar->addAction(nextAnnotationAction);
	annotationToolBar->addAction(prevAnnotationAction);
	annotationToolBar->addAction(browseCommunityAction);
	annotationToolBar->addAction(delAnnotationAction);
	toolBarMenu->addAction(annotationToolBar->toggleViewAction());
	
	presetToolBar = new QToolBar("Presets");
	presetToolBar->setObjectName("presets");
	presetToolBar->setOrientation(Qt::Horizontal);
	toolBarMenu->addAction(presetToolBar->toggleViewAction());
	
	toolToolBar = addToolBar("tools");
	toolToolBar->setObjectName("tools");
	toolToolBar->addAction(moveAction);
	toolToolBar->addAction(resizeAction);
	toolToolBar->addAction(zoomAction);
	toolToolBar->addAction(selectAction);
	toolToolBar->addAction(findAction);
	toolBarMenu->addAction(toolToolBar->toggleViewAction());
	
	settingToolBar = addToolBar("Global Settings");
	settingToolBar->setObjectName("setting");
	settingToolBar->addWidget(new QLabel("Display Width"));
	widthDial = new QSpinBox(this);
    widthDial->setMinimum(1);
    widthDial->setMaximum(1000000000);
    widthDial->setValue(128);
	settingToolBar->addWidget(widthDial);

	doubleDisplayWidth = new QPushButton("x2",this);
	settingToolBar->addWidget(doubleDisplayWidth);
	halveDisplayWidth = new QPushButton("/2",this);
	settingToolBar->addWidget(halveDisplayWidth);
	
	settingToolBar->addWidget(new QLabel("Scale"));
	scale = new QSpinBox(this);
    scale->setMinimum(1);
    scale->setMaximum(100000);
    scale->setValue(1);
    scale->setSingleStep(4);	
	settingToolBar->addWidget(scale);
	
	settingToolBar->addWidget(new QLabel("Zoom"));
	zoom = new QSpinBox(this);
    zoom->setMinimum(1);
    zoom->setMaximum(100000);
    zoom->setSingleStep(10);
    zoom->setValue(100);	
	settingToolBar->addWidget(zoom);
	
	settingToolBar->addWidget(new QLabel("Start"));
	startOffset = new QSpinBox(this);
    startOffset->setMinimum(1);
    startOffset->setMaximum(400000000);
    startOffset->setValue(1);
	settingToolBar->addWidget(startOffset);
	
	settingToolBar->addWidget(new QLabel("Display Length"));
	displayLength = new QSpinBox(this);
    displayLength->setMinimum(1000);
    displayLength->setMaximum(400000000);//something very large MAX_INT?
    displayLength->setSingleStep(1);
    displayLength->setValue(10000);	
	settingToolBar->addWidget(displayLength);
	
	settingToolBar->addWidget(new QLabel("Oligomer Length"));
	wordLength = new QSpinBox(this);
    wordLength->setMinimum(1);
    wordLength->setMaximum(5);
    wordLength->setSingleStep(1);
    wordLength->setValue(3);	
	settingToolBar->addWidget(wordLength);
	
	toolBarMenu->addAction(settingToolBar->toggleViewAction());
	addToolBar(Qt::RightToolBarArea,presetToolBar);
	addToolBar(Qt::LeftToolBarArea,toolToolBar);
	addToolBar(Qt::LeftToolBarArea,annotationToolBar);
	addToolBar(Qt::TopToolBarArea,settingToolBar);
	
}

void MainWindow::createDocks()
{	
	QDockWidget *infoDock = new QDockWidget("Information Display", this);
	infoDock->setObjectName("infodock");
	toolBarMenu->addAction(infoDock->toggleViewAction());
	infoDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	
    tabWidget = new QTabWidget(infoDock);
	infoDock->setWidget(tabWidget);
	textArea = new QTextEdit(tabWidget);
	tabWidget->setMinimumSize(60, 130);
    tabWidget->addTab(textArea, QString("Text Output"));
    highlighterTab = new QFrame();    
    seqEdit = new QLineEdit(highlighterTab);
    seqEdit->setMinimumWidth(400);
    similarityDial = new QSpinBox(highlighterTab);
    similarityDial->setValue(80);
    
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(new QLabel("Minimum Percent Similarity:", highlighterTab));
	vLayout->addWidget(similarityDial);
	vLayout->addWidget(new QLabel("Highlighted Sequence:", highlighterTab));
	vLayout->addWidget(seqEdit);
	highlighterTab->setLayout(vLayout);
	
    tabWidget->addTab(highlighterTab, QString("Highlighter Tab"));
	addDockWidget(Qt::BottomDockWidgetArea, infoDock);
	
	
	QWidget *filters = new QWidget;
	QVBoxLayout *filterLayout = new QVBoxLayout;
	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(new QListWidget, "Presets");
	//tabWidget->addTab(new QTreeView, "Advanced");
	
	QTableWidget *propsWidget = new QTableWidget(4,3);
	
	
	filterLayout->addWidget(tabWidget);
	filterLayout->addWidget(propsWidget);
	
	filters->setLayout(filterLayout);
	
	//}
	/*
	QMainWindow *presetLayout = new QMainWindow;
	filterDock->setWidget(presetLayout);
	presetLayout->addToolBar(presetToolBar);
	presetLayout->setCentralWidget(filters); 
	presetLayout->setMaximumWidth(300);
	presetLayout->setWindowFlags(Qt::Widget);
	
	addDockWidget(Qt::RightDockWidgetArea, filterDock);
	*/
	//{ settingDock:
		//QVBoxLayout *settingLayout = new QVBoxLayout;
		//QDockWidget *settingDock = new QDockWidget("Global Settings", this);
		//settingDock->setAllowedAreas(Qt::AllDockWidgetAreas);
		//settingDock->setWidget(new QTextEdit);
		//toolBarMenu->addAction(infoDock->toggleViewAction());
		//addDockWidget(Qt::BottomDockWidgetArea, settingDock);
	
	//}
}

void MainWindow::createStatusBar()
{
	//QLabel *statusTip = new QLabel("This is a status Tip",this);
	//QProgressBar *processStatus = new QProgressBar(this);
	//processStatus->setMinimumWidth(300);
	//processStatus->setMaximumWidth(300);
	//processStatus->setValue(75);
	//QLabel *processState = new QLabel("Loading...",this);
	
	//statusBar()->addPermanentWidget(processStatus);
	//statusBar()->addPermanentWidget(processState);
	statusBar()->showMessage(tr("Ready"));
}
void MainWindow::createConnections()
{
	/******Internal UI Logic*********/
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close())); 

	connect(glWidget, SIGNAL(IveBeenClicked(GLWidget*)), viewManager, SLOT(changeSelection(GLWidget*)));
	connect(glWidget2, SIGNAL(IveBeenClicked(GLWidget*)), viewManager, SLOT(changeSelection(GLWidget*)));


    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(this, SIGNAL(newFileOpen(QString)), viewManager, SLOT(changeFile(QString)));
    connect(this, SIGNAL(newFileOpen(QString)), glWidget->trackReader, SLOT(determineOutputFile(QString)));
    connect(this, SIGNAL(newFileOpen(QString)), glWidget2->trackReader, SLOT(determineOutputFile(QString)));

	connect(importAction, SIGNAL(triggered()), this, SLOT(openGtf()));
	connect(this, SIGNAL(newGtfFileOpen(QString)), viewManager, SLOT(addAnnotationDisplay(QString)));	
	connect(addAnnotationAction, SIGNAL(triggered()), viewManager, SLOT(addBookmark()));

    connect(moveAction, SIGNAL(triggered()), glWidget, SLOT(on_moveButton_clicked()));
    connect(selectAction, SIGNAL(triggered()), glWidget, SLOT(on_selectButton_clicked()));
    connect(resizeAction, SIGNAL(triggered()), glWidget, SLOT(on_resizeButton_clicked()));
    connect(moveAction, SIGNAL(triggered()), glWidget2, SLOT(on_moveButton_clicked()));
    connect(selectAction, SIGNAL(triggered()), glWidget2, SLOT(on_selectButton_clicked()));
    connect(resizeAction, SIGNAL(triggered()), glWidget2, SLOT(on_resizeButton_clicked()));
    
	connect( scale, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
	
	connect( doubleDisplayWidth, SIGNAL(clicked()), this, SLOT(doubleWidth()));
	connect( halveDisplayWidth, SIGNAL(clicked()), this, SLOT(halveWidth()));
	
	connect(verticalScrollBar, SIGNAL(valueChanged(int)), startOffset, SLOT(setValue(int)));
	connect(startOffset, SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));
	
	/****UNIVERSAL VARIABLES*******/
	connect(widthDial, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplaySize()));
    connect(zoom, SIGNAL(valueChanged(int)), glWidget, SLOT(changeZoom(int)));
	connect(widthDial, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplay()));
	connect(zoom, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplay()));
	connect(scale, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplay()));
	connect(scale, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplaySize()));
	connect(startOffset, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplay()));
	connect(displayLength, SIGNAL(valueChanged(int)), glWidget, SLOT(updateDisplay()));

	/****UNIVERSAL VARIABLES*******/
	connect(widthDial, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplaySize()));
    connect(zoom, SIGNAL(valueChanged(int)), glWidget2, SLOT(changeZoom(int)));
	connect(widthDial, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplay()));
	connect(zoom, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplay()));
	connect(scale, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplay()));
	connect(scale, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplaySize()));
	connect(startOffset, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplay()));
	connect(displayLength, SIGNAL(valueChanged(int)), glWidget2, SLOT(updateDisplay()));
   	/****PRESETS****/

	/****Specific Settings*****/
	connect( seqEdit, SIGNAL(textChanged(const QString&)), glWidget->highlight, SLOT(setHighlightSequence(const QString&)));
	connect( seqEdit, SIGNAL(textChanged(const QString&)), glWidget2->highlight, SLOT(setHighlightSequence(const QString&)));
	connect( similarityDial, SIGNAL(valueChanged(int)), glWidget->highlight, SLOT(setPercentSimilarity(int)));	
	connect( similarityDial, SIGNAL(valueChanged(int)), glWidget2->highlight, SLOT(setPercentSimilarity(int)));
	
	/****Print Signals*****/
	 connect( glWidget, SIGNAL(printText(QString)), textArea, SLOT(append(QString)));
	connect( glWidget2, SIGNAL(printText(QString)), textArea, SLOT(append(QString)));
	 connect( glWidget, SIGNAL(printHtml(QString)), textArea, SLOT(insertHtml(QString)));
	connect( glWidget2, SIGNAL(printHtml(QString)), textArea, SLOT(insertHtml(QString)));

	 connect( glWidget, SIGNAL(addGraphMode(AbstractGraph*)), this, SLOT(addDisplayActions(AbstractGraph*)));
	connect( glWidget2, SIGNAL(addGraphMode(AbstractGraph*)), this, SLOT(addDisplayActions(AbstractGraph*)));
	
	connect( glWidget, SIGNAL(addDivider()), this, SLOT(addDisplayDivider()));

}

UiVariables MainWindow::getDisplayVariables()
{
	UiVariables var = UiVariables();

	var.horizontalScrollBar = horizontalScrollBar;
	var.verticalScrollBar = verticalScrollBar;
	var.sizeDial = displayLength;
    var.widthDial = widthDial;
    var.startDial = startOffset;
    var.scaleDial = scale;
    var.zoomDial = zoom;
    var.oligDial = wordLength;
    
    return var;
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,"Open Sequence File", 
		"", 
		"FASTA files (*.fa);; Image files (*.png *.xpm *.jpg);; Text files (*.txt);; All files (*)"
	);
	
    if (!fileName.isEmpty()) 
		 emit newFileOpen(fileName);
}

void MainWindow::open(QString fileName)
{
    if (!fileName.isEmpty()) 
		 emit newFileOpen(fileName);
}

void MainWindow::changeWindowName(std::string name)
{
	string title = "Skittle - ";
	title.append(name);
	setWindowTitle(QApplication::translate("SkittleGUI", title.c_str(), 0, QApplication::UnicodeUTF8));
}

void MainWindow::openGtf()
{
	QString fileName = QFileDialog::getOpenFileName(this,"Open GTF File", "", tr("Annotation files (*.gtf);; Any files (*)"));
    
    if (!fileName.isEmpty()) 
		 emit newGtfFileOpen(fileName);
}

void MainWindow::changeScale(int newScale)
{
	if(newScale < 1)
		newScale = 1;
	
	if(oldScale != newScale)
	{
		widthDial->setSingleStep(newScale);		
		int display_width = widthDial->value() / oldScale;
		display_width = max( 1, display_width);
		int display_size = displayLength->value() / oldScale;
		display_size = max( 1, display_size);
		scale->setValue(newScale);
		widthDial->setValue( display_width * newScale );
		displayLength->setValue(display_size*newScale);
		oldScale = newScale;
	}
}

void MainWindow::doubleWidth()
{
		widthDial->setValue( 2 * widthDial->value() );
}

void MainWindow::halveWidth()
{
		widthDial->setValue( (int)(0.5 * widthDial->value()) );
}


void MainWindow::updateProgress(int val)
{
	//processStatus->setValue(val);
}
void MainWindow::updateState(QString state){
	//processState->setText(state);
}
void MainWindow::updateStatus(QString tip){
	statusBar()->showMessage(tip);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	writeSettings();
	event->accept();
//	QMainWindow::closeEvent(event);
}

void MainWindow::readSettings()
{
	glWidget->print("Reading User Settings");
	QSettings settings("Skittle", "Preferences");
	settings.beginGroup("mainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	settings.endGroup();
}

void MainWindow::writeSettings()
{
	glWidget->print("Writing Settings");
	QSettings settings("Skittle", "Preferences");
	settings.beginGroup("mainWindow");
	settings. setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.endGroup();
}
