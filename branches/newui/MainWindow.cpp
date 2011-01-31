#include <QtGui>
#include <QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QCheckBox>

#include <iostream>
#include <string>
#include <sstream>

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
#include "TextureCanvas.h"


MainWindow::MainWindow()
{
	textArea = NULL;
	setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
	setWindowTitle( "Skittle Genome Visualizer");
	createActions();
	createMenus();
	createToolbars();
	createDocks();
	createStatusBar();
	createUiConnections();
	oldScale = 1;
    oldWidth = 128;
	
	viewManager	= new ViewManager(this, getDisplayVariables());
	setCentralWidget(viewManager);  
	createFileConnections();

	readSettings();
    setWindowIcon(QIcon(":/skittle.svg"));
	
	//openAction->trigger();
}

void MainWindow::addDisplayActions(AbstractGraph* display)
{
	if(display->actionLabel.size() > 0)
	{
		QAction* presetAction = new QAction(QString(display->actionLabel.c_str()),this);
		presetAction->setStatusTip(QString(display->actionTooltip.c_str()));
		presetAction->setToolTip(QString(display->actionTooltip.c_str()));
		//presetAction->setData(QString(display->actionData.c_str()));
		display->toggleButton = presetAction;
		connect(presetAction,SIGNAL(triggered()),display, SLOT(toggleVisibility()));
		connect(display, SIGNAL(deleteButton(QAction*)), this, SLOT(removeButton(QAction*)));
	
		presetMenu->addAction(presetAction);
		presetToolBar->addAction(presetAction);
		display->setButtonFont();
	}
	else
	{
		print("Tried to add display mode with no label, aborting...");
	}
}

void MainWindow::removeButton(QAction* presetAction)
{
	presetMenu->removeAction(presetAction);
	presetToolBar->removeAction(presetAction);
}

void MainWindow::addDisplayDivider()
{
	presetToolBar->addSeparator();	
}

void MainWindow::createActions()
{
	moveAction = new QAction("Move",this);	
	moveAction->setToolTip(QString("Horizontal and vertical scroll"));
	resizeAction = new QAction("Resize",this);	
	resizeAction->setToolTip(QString("Adjust Width and Start position"));
	zoomAction = new QAction("Zoom",this);	
	zoomAction->setToolTip(QString("Shift+Click to zoom out"));
	selectAction = new QAction("Select",this);	
	selectAction->setToolTip(QString("Displays index and sequence information"));
	findAction = new QAction("&Find",this);	
	addAnnotationAction = new QAction("Add Annotation",this);	
	//useTexturesAction = new QAction("Use Hardware Acceleration", this);
	//useTexturesAction->setCheckable(true);
	//nextAnnotationAction = new QAction("Next Annotation",this);	
	//prevAnnotationAction = new QAction("Previous Annotation",this);	
	//browseCommunityAction = new QAction("Browse Community Research",this);	
	//delAnnotationAction = new QAction("Delete Current Bookmark",this);	
	
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
	
	addViewAction = new QAction("New Window",this);	
	
	openAction = new QAction("&Open File",this);
	openAction->setStatusTip("Open a Sequence File");
	
	importAction = new QAction("&Import GTF Annotation File",this);
	importAction->setStatusTip("Open GTF / GFF Annotation File");
	
	exitAction = new QAction("E&xit",this);
	helpAction = new QAction("Online &Help",this);
	aboutQtAct = new QAction("About Qt", this);
	exitAction->setStatusTip("Close Program");
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu("&File");
	fileMenu->addAction(openAction);
	fileMenu->addAction(addViewAction);
	fileMenu->addSeparator();
	fileMenu->addAction(importAction);
	fileMenu->addAction(exitAction);
	/*searchMenu = menuBar()->addMenu("&Search");
	searchMenu->addAction(findSequenceAction);
	searchMenu->addAction(findNextAction);
	searchMenu->addAction(findPrevAction);
	searchMenu->addAction(hilightResultsAction);*/
	viewMenu = menuBar()->addMenu("&View");
	//viewMenu->addAction(useTexturesAction);
	toolBarMenu = viewMenu->addMenu("ToolBar");
	presetMenu = viewMenu->addMenu("Presets");
	
	annotationMenu = menuBar()->addMenu("&Annotations");
	annotationMenu->addAction(addAnnotationAction);
	//annotationMenu->addAction(nextAnnotationAction);
	//annotationMenu->addAction(prevAnnotationAction);
	//annotationMenu->addAction(browseCommunityAction);
	//annotationMenu->addAction(delAnnotationAction);
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
	
	QMenu* helpMenu = menuBar()->addMenu("&Help");
	helpMenu->addAction(helpAction);
	helpMenu->addAction(aboutQtAct);
	
}

void MainWindow::createToolbars()
{
	QFont boldFont = QFont();
	boldFont.setBold(true);

	annotationToolBar = addToolBar("Files");
	annotationToolBar->setObjectName("file");
	annotationToolBar->addAction(openAction);
	annotationToolBar->addAction(addViewAction);
	annotationToolBar->addAction(addAnnotationAction);
	//annotationToolBar->addAction(nextAnnotationAction);
	//annotationToolBar->addAction(prevAnnotationAction);
	//annotationToolBar->addAction(browseCommunityAction);
	//annotationToolBar->addAction(delAnnotationAction);
	toolBarMenu->addAction(annotationToolBar->toggleViewAction());
	
	presetToolBar = new QToolBar("Presets");
	presetToolBar->setObjectName("presets");
	presetToolBar->setOrientation(Qt::Horizontal);
	toolBarMenu->addAction(presetToolBar->toggleViewAction());
	
	toolToolBar = addToolBar("Tools");
	toolToolBar->setObjectName("Tools");
	toolToolBar->addAction(moveAction);
	toolToolBar->addAction(resizeAction);
	toolToolBar->addAction(zoomAction);
	toolToolBar->addAction(selectAction);
	toolToolBar->addAction(findAction);
	toolBarMenu->addAction(toolToolBar->toggleViewAction());
	
	settingToolBar = addToolBar("Global Settings");
	settingToolBar->setObjectName("setting");

	QLabel* activeW = new QLabel("Active Window:");
	activeW->setFont(boldFont);	
	settingToolBar->addWidget(activeW);
	settingToolBar->addSeparator();
	
	settingToolBar->addWidget(new QLabel("Width"));
	widthDial = new QSpinBox(this);
    widthDial->setMinimum(1);
    widthDial->setMaximum(1000000000);
    widthDial->setValue(128);
    widthDial->setSuffix(" bp");
    widthDial->setButtonSymbols(QAbstractSpinBox::NoButtons);
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
    scale->setSuffix(" bp/pixel");
    scale->setButtonSymbols(QAbstractSpinBox::NoButtons);
	settingToolBar->addWidget(scale);
	
	settingToolBar->addWidget(new QLabel("Zoom"));
	zoom = new QSpinBox(this);
    zoom->setMinimum(1);
    zoom->setMaximum(100000);
    zoom->setSingleStep(10);
    zoom->setValue(100);	
    zoom->setButtonSymbols(QAbstractSpinBox::NoButtons);
	settingToolBar->addWidget(zoom);
	
	settingToolBar->addWidget(new QLabel("Start Index"));
	startOffset = new QSpinBox(this);
    startOffset->setMinimum(1);
    startOffset->setMaximum(400000000);
    startOffset->setValue(1);
    startOffset->setButtonSymbols(QAbstractSpinBox::NoButtons);
	settingToolBar->addWidget(startOffset);
	
	settingToolBar->addWidget(new QLabel("Display Length"));
	displayLength = new QSpinBox(this);
    displayLength->setMinimum(1000);
    displayLength->setMaximum(400000000);//something very large MAX_INT?
    displayLength->setSingleStep(1000);
    displayLength->setValue(10000);	
    displayLength->setSuffix(" bp");
    displayLength->setButtonSymbols(QAbstractSpinBox::NoButtons);
	settingToolBar->addWidget(displayLength);

	
	//settingToolBar->addSeparator();

	QLabel* multiW = new QLabel("Multiple Windows:");
	multiW->setFont(boldFont);	
	settingToolBar->addWidget(multiW);
	settingToolBar->addSeparator();
	//QActionGroup* multiGroup = new QActionGroup(this);
	syncCheckBox = new QCheckBox("Synchronize Views", this);
    syncCheckBox->setCheckState(Qt::Checked);	
	//multiGroup->addWidget(syncCheckBox);
	settingToolBar->addWidget(syncCheckBox);
	settingToolBar->addWidget( new QLabel("Offsets:"));
	
	
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
    tabWidget->addTab(textArea, QString("Text Output"));
	addDockWidget(Qt::BottomDockWidgetArea, infoDock);
	
	/*
	QWidget *filters = new QWidget;
	QVBoxLayout *filterLayout = new QVBoxLayout;
	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(new QListWidget, "Presets");
	//tabWidget->addTab(new QTreeView, "Advanced");
	QTableWidget *propsWidget = new QTableWidget(4,3);
	
	filterLayout->addWidget(tabWidget);
	filterLayout->addWidget(propsWidget);
	filters->setLayout(filterLayout);*/
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
void MainWindow::createUiConnections()
{	/******Internal UI Logic*********/
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close())); 
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(helpDialog()));
    
	//connect( scale, SIGNAL(valueChanged(int)),this, SLOT(changeScale(int)));
    connect( scale, SIGNAL(editingFinished()), this, SLOT(changeScale()));
    
	connect( doubleDisplayWidth, SIGNAL(clicked()), this, SLOT(doubleWidth()));
	connect( halveDisplayWidth, SIGNAL(clicked()), this, SLOT(halveWidth()));
    connect( widthDial, SIGNAL(editingFinished()), this, SLOT(changeWidth()));
    connect( halveDisplayWidth, SIGNAL(clicked()), widthDial, SIGNAL(editingFinished()));
    connect( doubleDisplayWidth, SIGNAL(clicked()), widthDial, SIGNAL(editingFinished()));
	
}
void MainWindow::createFileConnections()
{
	//Consider making this a function-call chain.... or handling this specifically, in a thread.
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(this, SIGNAL(newFileOpen(QString)), viewManager, SLOT(changeFile(QString)));

	connect(importAction, SIGNAL(triggered()), this, SLOT(openGtf()));
	connect(this, SIGNAL(newGtfFileOpen(QString)), viewManager, SLOT(addAnnotationDisplay(QString)));	
	connect(addAnnotationAction, SIGNAL(triggered()), viewManager, SLOT(addBookmark()));
}

UiVariables MainWindow::getDisplayVariables()
{
	UiVariables var(textArea);

	var.sizeDial = displayLength;
    var.widthDial = widthDial;
    var.startDial = startOffset;
    var.scaleDial = scale;
    var.zoomDial = zoom;
    
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
	string title = "Skittle Genome Visualizer";
	title.append(name);
	setWindowTitle( "Skittle Genome Visualizer");
}

void MainWindow::openGtf()
{
	QString fileName = QFileDialog::getOpenFileName(this,"Open GTF File", "", tr("Annotation files (*.gtf);; Any files (*)"));
    
    if (!fileName.isEmpty()) 
		 emit newGtfFileOpen(fileName);
}
void MainWindow::changeWidth(int newWidth){
    if (newWidth != oldWidth){
        int widthChange = abs(newWidth - oldWidth);
        
        int newScale = 1;
        int displayWidth = newWidth / scale->value();
        if ( displayWidth < 1 || displayWidth > TextureCanvas::maxSaneWidth){
            
            newScale = int(max(double(1.0),double(oldScale) * ( double(newWidth) / double(oldWidth))));
            scale->setValue(newScale);
            oldScale = newScale;
        }
        
        widthDial->setValue(newWidth);
    }
    oldWidth = newWidth;
    
}
void MainWindow::changeWidth(){
    changeWidth(widthDial->value());
}
void MainWindow::changeScale(int newScale)
{
	//std::stringstream ss1;
	//ss1 << "changeScale: " << newScale;
	//textArea->append(QString( ss1.str().c_str() ));	

	if(newScale < 1)
		newScale = 1;
	
	if(oldScale != newScale)
	{
		//increment the width step by scale
        widthDial->setSingleStep(newScale);		
        
        //
		int display_width = widthDial->value() / oldScale;
                
		display_width = max( 1, display_width);
		int display_size = displayLength->value() / oldScale;
		display_size = max( 1, display_size);
        
		widthDial->setValue( display_width * newScale );
		scale->setValue(newScale);
		displayLength->setValue(display_size*newScale);
		oldScale = newScale;
	}
}
void MainWindow::changeScale()
{
    changeScale(scale->value());
}

void MainWindow::doubleWidth()
{
		widthDial->setValue( 2 * widthDial->value() );
		//viewManager->updateCurrentDisplay();
}

void MainWindow::halveWidth()
{
		widthDial->setValue( (int)(0.5 * widthDial->value()) );
		//viewManager->updateCurrentDisplay();
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
	print("Reading User Settings");
	QSettings settings("Skittle", "Preferences");
	settings.beginGroup("mainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	settings.endGroup();
}

void MainWindow::writeSettings()
{	
	print("Writing Settings");
	QSettings settings("Skittle", "Preferences");
	settings.beginGroup("mainWindow");
	settings. setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.endGroup();
}

void MainWindow::helpDialog()
{
	/*QMessageBox help(this);
	help.setText("Up to date help information can be found at http://dnaskittle.com/faq.");
	help.exec();*/
	
    QMessageBox mb(this);

    QString c("About Qt");
    mb.setWindowTitle(c);
    mb.setText(QString(
        "<h3>About Skittle</h3>"
        "<p>Skittle is an Open Source program for browsing genome sequences.</p>"
        "<p>Up to date help information is available online at "
		"<a href=\"http://dnaskittle.com/faq/\">http://DNASkittle.com/faq/</a> </p>"
        "<p>For those new to the program, there is a tutorial at "
        "<a href=\"http://dnaskittle.com/getting-started/\">http://DNASkittle.com/getting-started/</a>"
		" that walks through the basic tools of Skittle and some real world"
		" genome patterns that can be identified with Skittle.</p>"
		));
    /** /    
#ifndef QT_NO_IMAGEFORMAT_XPM
    QImage logo(qtlogo_xpm);
#else
    QImage logo;
#endif

    if (qGray(mb.palette().color(QPalette::Active, QPalette::Text).rgb()) >
        qGray(mb.palette().color(QPalette::Active, QPalette::Base).rgb()))
    {
        // light on dark, adjust some colors
        logo.setColor(0, 0xffffffff);
        logo.setColor(1, 0xff666666);
        logo.setColor(2, 0xffcccc66);
        logo.setColor(4, 0xffcccccc);
        logo.setColor(6, 0xffffff66);
        logo.setColor(7, 0xff999999);
        logo.setColor(8, 0xff3333ff);
        logo.setColor(9, 0xffffff33);
        logo.setColor(11, 0xffcccc99);
    }
    QPixmap pm = QPixmap::fromImage(logo);
    if (!pm.isNull())
        mb.setIconPixmap(pm);/**/
#if defined(Q_OS_WINCE)
    mb.setDefaultButton(mb.addButton(QMessageBox::Ok));
#else
    mb.addButton(QMessageBox::Ok);
#endif
    mb.exec();
}

/**********Print Functions**********/
void MainWindow::print(const char* str)
{
	if(textArea != NULL)
		textArea->append(QString(str));
}
void MainWindow::print(QString str)
{
    if(textArea != NULL)
		textArea->append(str);
}

void MainWindow::reportFinished(){
	print("Report Finished");
}
