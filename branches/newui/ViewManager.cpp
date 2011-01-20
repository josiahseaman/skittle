//ViewManager.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "MainWindow.h"
#include "MdiChildWindow.h"
#include <QtGui/QScrollBar>
#include <algorithm>

ViewManager::ViewManager(MainWindow* window, UiVariables gui)
	: QMdiArea(window),
	ui(gui)
{
	mainWindow = window;
	activeWidget = NULL;
	
	setBackgroundRole(QPalette::Dark);

	createConnections();
	
	addNewView();
	views[0]->showMaximized();
}

void ViewManager::createConnections()
{	
	connect(mainWindow->addViewAction, SIGNAL(triggered()), this, SLOT(addNewView()));
	connect(mainWindow->syncCheckBox, SIGNAL(stateChanged(int)), this, SLOT(handleWindowSync()));
	
	/****CONNECT ui VARIABLES*******/ 
	
	connect(ui.widthDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));	connect(ui.zoomDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
	connect(ui.scaleDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
	connect(ui.startDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
	connect(ui.sizeDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
	
	//TODO: find where these belong;
    //~ connect(ui.widthDial, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
	//~ connect(ui.zoomDial,  SIGNAL(valueChanged(int)), this, SLOT(zoomChanged(int)));
	//~ connect(ui.scaleDial, SIGNAL(valueChanged(int)), this, SLOT(scaleChanged(int)));
	//~ connect(ui.startDial, SIGNAL(valueChanged(int)), this, SLOT(offsetChanged(int)));
	//~ connect(ui.sizeDial,  SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));
	
	
	
}

void ViewManager::connectLocalCopy(GLWidget* active, UiVariables local)
{	
	//The Following has been moved to GLWidget::createConnections()
	//testing
	//~ connect(local.widthDial, SIGNAL(editingFinished ()), active, SLOT(reportOnFinish()));
	
	/****CONNECT LOCAL VARIABLES*******/ 
	//this should be handled internally by glwidget, in the constructor, or anytime a view is added to the glwidget, not here.
	//~ connect(local.widthDial, SIGNAL(editingFinished()), active, SLOT(updateDisplaySize()));
	//~ connect(local.widthDial, SIGNAL(editingFinished()), active, SLOT(updateDisplay()));
	//~ connect(local.zoomDial,  SIGNAL(valueChanged(int)), active, SLOT(changeZoom(int)));
	//~ connect(local.zoomDial,  SIGNAL(editingFinished()), active, SLOT(updateDisplay()));
	//~ connect(local.scaleDial, SIGNAL(editingFinished()), active, SLOT(updateDisplay()));
	//~ connect(local.scaleDial, SIGNAL(valueChanged(int)), active, SLOT(updateDisplaySize()));
	//~ connect(local.startDial, SIGNAL(editingFinished()), active, SLOT(updateDisplay()));
	//~ connect(local.sizeDial,  SIGNAL(editingFinished()), active, SLOT(updateDisplay()));
 
}

void ViewManager::uiToGlwidgetConnections(GLWidget* active)
{
	connect(dynamic_cast<MdiChildWindow*>(active->parent), SIGNAL(IveBeenClicked(MdiChildWindow*)),
		this, SLOT(changeSelection(MdiChildWindow*)));
	
    connect(mainWindow->moveAction, SIGNAL(triggered()), active, SLOT(on_moveButton_clicked()));
    connect(mainWindow->selectAction, SIGNAL(triggered()), active, SLOT(on_selectButton_clicked()));
    connect(mainWindow->findAction, SIGNAL(triggered()), active, SLOT(on_findButton_clicked()));
    connect(mainWindow->resizeAction, SIGNAL(triggered()), active, SLOT(on_resizeButton_clicked()));
    connect(mainWindow->zoomAction, SIGNAL(triggered()), active, SLOT(on_zoomButton_clicked()));
	
	connect( active, SIGNAL(addGraphMode(AbstractGraph*)), mainWindow, SLOT(addDisplayActions(AbstractGraph*)));
	
	connect( active, SIGNAL(addDivider()), mainWindow, SLOT(addDisplayDivider()));
	active->createButtons();

}

//public slots
GLWidget* ViewManager::addNewView(bool suppressOpen)
{
	UiVariables localDials = copyUi();
	broadcastPublicValues(localDials);
	
	MdiChildWindow* child = new MdiChildWindow(localDials, ui.startDial, mainWindow->tabWidget);//TODO: figure out a better way to manage startDial
	connect( child, SIGNAL(subWindowClosing(MdiChildWindow*)), this, SLOT(closeSubWindow(MdiChildWindow*)));
    addSubWindow(child);
    child->show();
    views.push_back(child);
	
	GLWidget* newGlWidget = child->glWidget;
	connectLocalCopy(newGlWidget,  localDials);
	uiToGlwidgetConnections(newGlWidget);
	connectVariables(newGlWidget, localDials);
	
	changeSelection(child);
	if(suppressOpen == false)
		mainWindow->openAction->trigger();
	return newGlWidget;
}

void ViewManager::changeSelection(MdiChildWindow* parent)
{
	GLWidget* current = parent->glWidget;
	if(current == activeWidget)
		return;
		
	int tabIndex = mainWindow->tabWidget->currentIndex();
	if(activeWidget != NULL)
	{
		if(mainWindow->syncCheckBox->isChecked() == false )
		{
			disconnectVariables(activeWidget, vars(activeWidget));
			connectVariables(current, vars(current));
		}
		parent->hideSettingsTabs();
	}
	
	parent->showSettingsTabs();
	mainWindow->tabWidget->setCurrentIndex(tabIndex);
		
	activeWidget = current;		
	activeWidget->setTotalDisplayWidth();	
}

void ViewManager::closeSubWindow(MdiChildWindow* closing)
{
	vector<MdiChildWindow*>::iterator it;
	it = std::find(views.begin(), views.end(), closing);
	views.erase(it);
	if(closing->glWidget == activeWidget)
	{
		if( views.size() > 0)
			changeSelection(views[0]);
		else
			activeWidget = NULL;
	}
}

void ViewManager::changeFile(QString fileName)
{
	if(!fileName.isEmpty() )
	{
		if(activeWidget == NULL )
		{
			addNewView(true);
		}
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
	//local.start changes
	UiVariables local = vars(activeWidget);
	int set = max(0, val - local.offsetDial->value());
	ui.startDial->setValue(set);
	//ui.print("changePublicStart: ", set);	
}

//PRIVATE FUNCTIONS//
void ViewManager::broadcastLocalValues(UiVariables local)
{
	ui.startDial->setValue(local.startDial->value());
	
	ui.sizeDial->setValue(local.sizeDial->value());
	ui.widthDial->setValue(local.widthDial->value());
	ui.scaleDial->setValue(local.scaleDial->value());
	ui.zoomDial->setValue(local.zoomDial->value());
}

void ViewManager::broadcastPublicValues(UiVariables local)
{
	local.startDial->setValue(ui.startDial->value());
	
	local.sizeDial->setValue(ui.sizeDial->value());
	local.widthDial->setValue(ui.widthDial->value());
	local.scaleDial->setValue(ui.scaleDial->value());
	local.zoomDial->setValue(ui.zoomDial->value());
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

	QSpinBox* offsetDial = new QSpinBox(this);
    offsetDial->setMinimum(-40000000);
    offsetDial->setMaximum(40000000);
    offsetDial->setValue(0);
    offsetDial->setSingleStep(widthDial->value());
    mainWindow->settingToolBar->addWidget(offsetDial);
    //offsetDial->hide();
    
	UiVariables localDials(ui.textArea);
	localDials.sizeDial  = sizeDial;
    localDials.widthDial = widthDial;
    localDials.startDial = startDial;
    localDials.scaleDial = scaleDial;
    localDials.zoomDial  = zoomDial;
    localDials.offsetDial  = offsetDial;
    
	return localDials;
}

void ViewManager::printNum(int num)
{
	ui.print("Global:  ", num);
}

void ViewManager::printNum2(int num)
{
	ui.print("Local:   ", num);
}

//the mother load!
//implement in glWidget
void ViewManager::connectVariables(GLWidget* active, UiVariables local)
{
	connect(local.sizeDial	, SIGNAL(valueChanged(int)), ui.sizeDial	, SLOT(setValue(int)));
	//connect(ui.sizeDial		, SIGNAL(valueChanged(int)), local.sizeDial	, SLOT(setValue(int)));

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
	connect(active,           SIGNAL(scaleChanged(int)), local.scaleDial,    SLOT(setValue(int)));

	connect(local.zoomDial	, SIGNAL(valueChanged(int)), ui.zoomDial	, SLOT(setValue(int)));
	connect(ui.zoomDial		, SIGNAL(valueChanged(int)), local.zoomDial	, SLOT(setValue(int)));
    connect(active, SIGNAL(scaleChangedSmart(int)), this, SLOT(scaleChangedSmart(int)));
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
}

UiVariables ViewManager::vars(GLWidget* active)
{
	return dynamic_cast<MdiChildWindow*>(active->parent)->ui;
}
void ViewManager::updateCurrentDisplay(){
	//activeWidget->reportOnFinish();
	
	if (mainWindow->syncCheckBox->isChecked()){
		
		for(int i = 0; i < (int)views.size(); ++i) {
			views[i]->glWidget->updateDisplay();
		}
	}
	else {
		activeWidget->updateDisplay();
	}
}

void ViewManager::scaleChangedSmart(int newScale){
    mainWindow->changeScale(newScale);
}
