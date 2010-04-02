//ViewManager.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "MainWindow.h"
#include <QtGui/QScrollBar>

ViewManager::ViewManager(MainWindow* window, UiVariables gui)
{
	mainWindow = window;
	ui = gui;
	activeWidget = NULL;

	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );
	
	setBackgroundRole(QPalette::Dark);
	glWidget = new GLWidget(ui, this);
	//views.push_back(glWidget);
	uiToGlwidgetConnections(glWidget);
	//glWidget2 = NULL;//new GLWidget(ui, this);
	
	changeSelection(glWidget);

	QFrame* subFrame = new QFrame;
	hLayout = new QHBoxLayout;
	hLayout->addWidget(glWidget);
	//hLayout->addWidget(glWidget2);
	hLayout->addWidget(verticalScrollBar);
	subFrame->setLayout(hLayout);
	
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(subFrame);
	vLayout->addWidget(horizontalScrollBar);
	setLayout(vLayout);
	
	createConnections();
}

void ViewManager::createConnections()
{
	connect(ui.sizeDial, SIGNAL(valueChanged(int)), this, SLOT(setPageSize()) );	
	
	connect(verticalScrollBar, SIGNAL(valueChanged(int)), ui.startDial, SLOT(setValue(int)));
	connect(ui.startDial, SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));
	
	connect(mainWindow->addViewAction, SIGNAL(triggered()), this, SLOT(addNewView()));
}

void ViewManager::uiToGlwidgetConnections(GLWidget* active)
{
	connect(active, SIGNAL(IveBeenClicked(GLWidget*)), this, SLOT(changeSelection(GLWidget*)));
	connect(mainWindow, SIGNAL(newFileOpen(QString)), active->trackReader, SLOT(determineOutputFile(QString)));
	
    connect(mainWindow->moveAction, SIGNAL(triggered()), active, SLOT(on_moveButton_clicked()));
    connect(mainWindow->selectAction, SIGNAL(triggered()), active, SLOT(on_selectButton_clicked()));
    connect(mainWindow->resizeAction, SIGNAL(triggered()), active, SLOT(on_resizeButton_clicked()));
    
	/****UNIVERSAL VARIABLES*******/
	connect(mainWindow->widthDial, SIGNAL(valueChanged(int)),     active, SLOT(updateDisplaySize()));
    connect(mainWindow->zoom, SIGNAL(valueChanged(int)),          active, SLOT(changeZoom(int)));
	connect(mainWindow->widthDial, SIGNAL(valueChanged(int)),     active, SLOT(updateDisplay()));
	connect(mainWindow->zoom, SIGNAL(valueChanged(int)),          active, SLOT(updateDisplay()));
	connect(mainWindow->scale, SIGNAL(valueChanged(int)),         active, SLOT(updateDisplay()));
	connect(mainWindow->scale, SIGNAL(valueChanged(int)),         active, SLOT(updateDisplaySize()));
	connect(mainWindow->startOffset, SIGNAL(valueChanged(int)),   active, SLOT(updateDisplay()));
	connect(mainWindow->displayLength, SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
	
	/****Specific Settings**** TODO: Move to display constructors "needs()"*/
	connect( mainWindow->seqEdit, SIGNAL(textChanged(const QString&)), active->highlight, SLOT(setHighlightSequence(const QString&)));
	connect( mainWindow->similarityDial, SIGNAL(valueChanged(int)), active->highlight, SLOT(setPercentSimilarity(int)));	
	
	/****Print Signals*****/
	connect( active, SIGNAL(printText(QString)), mainWindow->textArea, SLOT(append(QString)));
	connect( active, SIGNAL(printHtml(QString)), mainWindow->textArea, SLOT(insertHtml(QString)));

	connect( active, SIGNAL(addGraphMode(AbstractGraph*)), mainWindow, SLOT(addDisplayActions(AbstractGraph*)));
	
	connect( active, SIGNAL(addDivider()), mainWindow, SLOT(addDisplayDivider()));
	active->createButtons();	

}

//public slots
void ViewManager::addNewView()
{
	GLWidget* newGlWidget = new GLWidget(ui, this);
	views.push_back(newGlWidget);
	hLayout->insertWidget((int)views.size(), newGlWidget); 
	uiToGlwidgetConnections(newGlWidget);
	changeSelection(newGlWidget);
	mainWindow->openAction->trigger();
}

void ViewManager::changeSelection(GLWidget* current)
{
	if(activeWidget != NULL)
	{
		disconnectWidget();
	}
		activeWidget = current;
		connectWidget();
		
		setPageSize();
		activeWidget->setTotalDisplayWidth();
	//}
}

void ViewManager::changeFile(QString fileName)
{
	if(activeWidget != NULL)
	{
		activeWidget->loadFile(fileName);
	}
}

void ViewManager::setPageSize()
{
	if( activeWidget != NULL)
		verticalScrollBar->setMaximum( max(0, (int)(activeWidget->seq()->size() - ui.widthDial->value()) ) );
	verticalScrollBar->setPageStep(ui.sizeDial->value());
}

void ViewManager::connectWidget()
{		    	
	connect(horizontalScrollBar, SIGNAL(valueChanged(int)), activeWidget, SLOT(slideHorizontal(int)));
	connect(activeWidget, SIGNAL(xOffsetChange(int)), horizontalScrollBar, SLOT(setValue(int)));	
	//connect resizing horizontalScroll
	connect(activeWidget, SIGNAL(totalWidthChanged(int)), this, SLOT(setHorizontalWidth(int)));
}

void ViewManager::disconnectWidget()
{
	disconnect(horizontalScrollBar, SIGNAL(valueChanged(int)), activeWidget, SLOT(slideHorizontal(int)));
	disconnect(activeWidget, SIGNAL(xOffsetChange(int)), horizontalScrollBar, SLOT(setValue(int)));		
	//disconnect resizing horizontalScroll
	disconnect(activeWidget, SIGNAL(totalWidthChanged(int)), this, SLOT(setHorizontalWidth(int)));
}

void ViewManager::setHorizontalWidth(int val)
{
	horizontalScrollBar->setMaximum( max(0, val) );
}

void ViewManager::addAnnotationDisplay(QString fileName)
{
	if(activeWidget != NULL)
	{
		activeWidget->addAnnotationDisplay(fileName);
	}
}

void ViewManager::addBookmark()
{
	if(activeWidget != NULL)
	{
		activeWidget->trackReader->addBookmark();
	}
}
