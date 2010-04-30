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
	connect(mainWindow->syncCheckBox, SIGNAL(stateChanged(int)), this, SLOT(handleWindowSync()));
}

void ViewManager::connectLocalCopy(GLWidget* active, UiVariables local)
{	
	/****CONNECT LOCAL VARIABLES*******/
	connect(local.widthDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplaySize()));
	connect(local.zoomDial,  SIGNAL(valueChanged(int)), active, SLOT(changeZoom(int)));
	connect(local.widthDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
	connect(local.zoomDial,  SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
	connect(local.scaleDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
	connect(local.scaleDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplaySize()));
	connect(local.startDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
	connect(local.sizeDial,  SIGNAL(valueChanged(int)), active, SLOT(updateDisplay()));
    
}

void ViewManager::uiToGlwidgetConnections(GLWidget* active)
{
	connect(active, SIGNAL(IveBeenClicked(GLWidget*)), this, SLOT(changeSelection(GLWidget*)));
	
    connect(mainWindow->moveAction, SIGNAL(triggered()), active, SLOT(on_moveButton_clicked()));
    connect(mainWindow->selectAction, SIGNAL(triggered()), active, SLOT(on_selectButton_clicked()));
    connect(mainWindow->resizeAction, SIGNAL(triggered()), active, SLOT(on_resizeButton_clicked()));
	
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
	UiVariables localDials = copyUi();
	broadcastPublicValues(localDials);
	
	MdiChildWindow* child = new MdiChildWindow(localDials, ui.startDial);//TODO: figure out a better way to manage 
    addSubWindow(child);
    child->show();
    views.push_back(child);
	
	GLWidget* newGlWidget = child->glWidget;
	connectLocalCopy(newGlWidget,  localDials);
	uiToGlwidgetConnections(newGlWidget);
	connectVariables(newGlWidget, localDials);
	//connectOffset(newGlWidget, localDials);
	
	changeSelection(newGlWidget);
	mainWindow->openAction->trigger();
	return newGlWidget;
}

void ViewManager::changeSelection(GLWidget* current)
{
	if(activeWidget != NULL && mainWindow->syncCheckBox->isChecked() == false )
	{
		disconnectVariables(activeWidget, vars(activeWidget));
		connectVariables(current, vars(current));
	}
	//if(activeWidget != NULL)
	//	disconnectOffset(activeWidget, vars(activeWidget));	
	//connectOffset(current, vars(current));
		
	activeWidget = current;		
	activeWidget->setTotalDisplayWidth();	
}

void ViewManager::changeFile(QString fileName)
{
	if(activeWidget != NULL)
	{
		activeWidget->loadFile(fileName);
		activeWidget->trackReader->determineOutputFile(fileName);
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

void ViewManager::handleWindowSync()
{
	if( mainWindow->syncCheckBox->isChecked() )//reconnect all windows
	{
		for(int i = 0; i < (int)views.size(); ++i)
		{
			if(views[i]->glWidget != activeWidget)
				connectVariables(views[i]->glWidget, views[i]->ui);
		}
	}
	else//disconnect all windows but the current one
	{
		for(int i = 0; i < (int)views.size(); ++i)
		{
			if(views[i]->glWidget != activeWidget)
				disconnectVariables(views[i]->glWidget, views[i]->ui);
		}
	}
}

void ViewManager::changePublicStart(int val)
{
	//glWidget->print("changePublicStart: ", val);
	//local.start changes
	UiVariables local = vars(activeWidget);
	ui.startDial->setValue(max(0, local.startDial->value() - local.offsetDial->value()));
}

//PRIVATE FUNCTIONS//
void ViewManager::broadcastLocalValues(UiVariables local)
{
	ui.startDial->setValue(local.startDial->value());
	
	ui.sizeDial->setValue(local.sizeDial->value());
	ui.widthDial->setValue(local.widthDial->value());
	ui.scaleDial->setValue(local.scaleDial->value());
	ui.zoomDial->setValue(local.zoomDial->value());
	ui.oligDial->setValue(local.oligDial->value());
}

void ViewManager::broadcastPublicValues(UiVariables local)
{
	local.startDial->setValue(ui.startDial->value());
	
	local.sizeDial->setValue(ui.sizeDial->value());
	local.widthDial->setValue(ui.widthDial->value());
	local.scaleDial->setValue(ui.scaleDial->value());
	local.zoomDial->setValue(ui.zoomDial->value());
	local.oligDial->setValue(ui.oligDial->value());
}

UiVariables ViewManager::copyUi()
{
	QSpinBox* widthDial = new QSpinBox(this);
    widthDial->setMinimum(1);
    widthDial->setMaximum(1000000000);
    widthDial->setValue(128);
    widthDial->hide();
	
	QSpinBox* scaleDial = new QSpinBox(this);
    scaleDial->setMinimum(1);
    scaleDial->setMaximum(100000);
    scaleDial->setValue(1);
	scaleDial->hide();
	
	QSpinBox* zoomDial = new QSpinBox(this);
    zoomDial->setMinimum(1);
    zoomDial->setMaximum(100000);
    zoomDial->setValue(100);
	zoomDial->hide();
	
	QSpinBox* startDial = new QSpinBox(this);
    startDial->setMinimum(1);
    startDial->setMaximum(400000000);
    startDial->setValue(1);
    startDial->hide();
	
	QSpinBox* sizeDial = new QSpinBox(this);
    sizeDial->setMinimum(1000);
    sizeDial->setMaximum(400000000);//something very large MAX_INT?
    sizeDial->setValue(10000);	
    sizeDial->hide();
	
	QSpinBox* oligDial = new QSpinBox(this);
    oligDial->setMinimum(1);
    oligDial->setMaximum(5);
    oligDial->setValue(3);
    oligDial->hide();

	QSpinBox* offsetDial = new QSpinBox(this);
    offsetDial->setMinimum(-40000000);
    offsetDial->setMaximum(40000000);
    offsetDial->setValue(100);
    offsetDial->setSingleStep(widthDial->value());
    mainWindow->settingToolBar->addWidget(offsetDial);
    //offsetDial->hide();
    
	UiVariables localDials;
	localDials.sizeDial  = sizeDial;
    localDials.widthDial = widthDial;
    localDials.startDial = startDial;
    localDials.scaleDial = scaleDial;
    localDials.zoomDial  = zoomDial;
    localDials.oligDial  = oligDial;
    localDials.offsetDial  = offsetDial;
    
	return localDials;
}
/*
void ViewManager::connectOffset(GLWidget* active, UiVariables local)
{	
	connect(local.offsetDial, SIGNAL(valueChanged(int)), this, SLOT(changeLocalStartFromOffset(int)));
	connect(this, SIGNAL(startChangeFromOffset(int)), local.startDial, SLOT(setValue(int)));	
}

void ViewManager::disconnectOffset(GLWidget* active, UiVariables local)
{	
	disconnect(local.offsetDial, SIGNAL(valueChanged(int)), this, SLOT(changeLocalStartFromOffset(int)));
	disconnect(this, SIGNAL(startChangeFromOffset(int)), local.startDial, SLOT(setValue(int)));	
}
*/
void ViewManager::connectVariables(GLWidget* active, UiVariables local)
{
	connect(local.sizeDial	, SIGNAL(valueChanged(int)), ui.sizeDial	, SLOT(setValue(int)));
	connect(ui.sizeDial		, SIGNAL(valueChanged(int)), local.sizeDial	, SLOT(setValue(int)));

	connect(local.widthDial	, SIGNAL(valueChanged(int)), ui.widthDial	, SLOT(setValue(int)));
	connect(ui.widthDial	, SIGNAL(valueChanged(int)), local.widthDial, SLOT(setValue(int)));

	//OLD:widget -> local.start -> pub.start
	//NEW:widget -> local.start -> vMan.slots -> pub.start
	//connect(local.startDial	, SIGNAL(valueChanged(int)), ui.startDial	, SLOT(setValue(int)));
	connect(local.startDial , SIGNAL(valueChanged(int)), this, SLOT(changePublicStart(int)));
	connect(ui.startDial	, SIGNAL(valueChanged(int)), 
		dynamic_cast<MdiChildWindow*>(active->parent), SLOT(changeLocalStartFromPublicStart(int)));
	//connect(this, SIGNAL(startChangeFromPublicStart(int)), local.startDial, SLOT(setValue(int)));	

	connect(local.scaleDial	, SIGNAL(valueChanged(int)), ui.scaleDial	, SLOT(setValue(int)));
	connect(ui.scaleDial	, SIGNAL(valueChanged(int)), local.scaleDial, SLOT(setValue(int)));

	connect(local.zoomDial	, SIGNAL(valueChanged(int)), ui.zoomDial	, SLOT(setValue(int)));
	connect(ui.zoomDial		, SIGNAL(valueChanged(int)), local.zoomDial	, SLOT(setValue(int)));

	connect(local.oligDial	, SIGNAL(valueChanged(int)), ui.oligDial	, SLOT(setValue(int)));
	connect(ui.oligDial		, SIGNAL(valueChanged(int)), local.oligDial	, SLOT(setValue(int)));//TODO: Move this to individual basis
}

void ViewManager::disconnectVariables(GLWidget* active, UiVariables local)
{
	disconnect(local.sizeDial	, SIGNAL(valueChanged(int)), ui.sizeDial	, SLOT(setValue(int)));
	disconnect(ui.sizeDial		, SIGNAL(valueChanged(int)), local.sizeDial	, SLOT(setValue(int)));

	disconnect(local.widthDial	, SIGNAL(valueChanged(int)), ui.widthDial	, SLOT(setValue(int)));
	disconnect(ui.widthDial	, SIGNAL(valueChanged(int)), local.widthDial, SLOT(setValue(int)));

	disconnect(local.startDial , SIGNAL(valueChanged(int)), this, SLOT(changePublicStart(int)));
	disconnect(ui.startDial	, SIGNAL(valueChanged(int)), 
		dynamic_cast<MdiChildWindow*>(active->parent), SLOT(changeLocalStartFromPublicStart(int)));
	//disconnect(this, SIGNAL(startChangeFromPublicStart(int)), local.startDial, SLOT(setValue(int)));	
	
	//disconnect(local.offsetDial, SIGNAL(valueChanged(int)), this, SLOT(changeLocalStart(int)));

	disconnect(local.scaleDial	, SIGNAL(valueChanged(int)), ui.scaleDial	, SLOT(setValue(int)));
	disconnect(ui.scaleDial	, SIGNAL(valueChanged(int)), local.scaleDial, SLOT(setValue(int)));

	disconnect(local.zoomDial	, SIGNAL(valueChanged(int)), ui.zoomDial	, SLOT(setValue(int)));
	disconnect(ui.zoomDial		, SIGNAL(valueChanged(int)), local.zoomDial	, SLOT(setValue(int)));

	disconnect(local.oligDial	, SIGNAL(valueChanged(int)), ui.oligDial	, SLOT(setValue(int)));
	disconnect(ui.oligDial		, SIGNAL(valueChanged(int)), local.oligDial	, SLOT(setValue(int)));//TODO: Move this to individual basis
}

UiVariables ViewManager::vars(GLWidget* active)
{
	return dynamic_cast<MdiChildWindow*>(active->parent)->ui;
}

