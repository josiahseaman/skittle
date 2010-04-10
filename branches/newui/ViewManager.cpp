//ViewManager.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "MainWindow.h"
#include "MdiChildWindow.h"
#include <QtGui/QScrollBar>

ViewManager::ViewManager(MainWindow* window, UiVariables gui)
:QMdiArea(window)
{
	mainWindow = window;
	ui = gui;
	activeWidget = NULL;
	
	setBackgroundRole(QPalette::Dark);
	//glWidget = new GLWidget(ui, this);


	createConnections();
	
	glWidget = addNewView();
	changeSelection(glWidget);
}

void ViewManager::createConnections()
{	
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
GLWidget* ViewManager::addNewView()
{
	MdiChildWindow* child = new MdiChildWindow(ui);
    addSubWindow(child);
    child->show();
	
	GLWidget* newGlWidget = child->glWidget;
	uiToGlwidgetConnections(newGlWidget);
	
	views.push_back(newGlWidget);
	changeSelection(newGlWidget);
	mainWindow->openAction->trigger();
	return newGlWidget;
}

void ViewManager::changeSelection(GLWidget* current)
{
	if(activeWidget != NULL)
	{
		//disconnectWidget();
	}
		activeWidget = current;
		//connectWidget();
		
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
