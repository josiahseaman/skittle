//ViewManager.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "MainWindow.h"
#include "MdiChildWindow.h"
#include <QtGui/QScrollBar>
#include <algorithm>

/** *******************************************
ViewManager is the 'Multiple Document Interface' responsible for coordinating the existance of
multiple windows being open inside Skittle at any one time.  Each window is tied to one file.
The user has the option of keeping all windows in sync with each other or treating them as
different instances of the program through the mainWindow->syncCheckBox.

In the case wherethe windows are synced, changes to the dials are connected to each window and
propagate to display updates.  In the case where they are not synced, switching between active
windows actually changes the values of the dials to stored values.  This requires the creation
of local copies of UiVariables, one for each window (MdiChildWindow).  The conditional connections
go from the Global UiVariables set to each othe local UiVariables.  When a new window becomes
active, the old variables must be disconnected, the globals are updated, and the new active
variables are connected to them.  ViewManager contains all the logic for this dance of connections,
values, and updates.  Look at addNewView() to see what happens when a new player comes on stage.

Window Hierarchy:
MainWindow -> (1) Viewmanager -> (many) MdiChildWindow -> (1) GLWidget -> (1)FastaReader -> (1)File
MainWindow is at the top of a window hierarchy.  The center widget of MainWindow is a single
ViewManager which inherits from MdiArea.  The multiple document interface (MDI) can have multiple
MdiChildWindows.  Each MdiChildWindow has only one GLWidget tied to one file.  MainWindow is
the primary owner of the UiVariables object that is passed for signals all throughout the program.
********************************************/

ViewManager::ViewManager(MainWindow* window, UiVariables* gui)
    : QMdiArea(window)
{
    ui = gui;
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
	
    connect(ui->widthDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->zoomDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->scaleDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->startDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->sizeDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
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
    UiVariables* localDials = copyUi();
	broadcastPublicValues(localDials);
	
    MdiChildWindow* child = new MdiChildWindow(localDials, ui->startDial, mainWindow->tabWidget);//TODO: figure out a better way to manage startDial
	connect( child, SIGNAL(subWindowClosing(MdiChildWindow*)), this, SLOT(closeSubWindow(MdiChildWindow*)));
    addSubWindow(child);
    child->show();
    views.push_back(child);
	
    GLWidget* newGlWidget = child->glWidget;
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
    //local->start changes
    UiVariables* local = vars(activeWidget);
    int set = max(0, val - local->offsetDial->value());
    ui->changeStart(set);
//    ui->startDial->setValue(set);
    //ui->print("changePublicStart: ", set);
}

//PRIVATE FUNCTIONS//
void ViewManager::broadcastPublicValues(UiVariables* local)
{
    local->startDial->setValue(ui->startDial->value());
    local->sizeDial->setValue(ui->sizeDial->value());
    local->widthDial->setValue(ui->widthDial->value());
    local->scaleDial->setValue(ui->scaleDial->value());
    local->zoomDial->setValue(ui->zoomDial->value());
}

UiVariables* ViewManager::copyUi()
{
    UiVariables* localDials = new UiVariables(ui->textArea);
    localDials->widthDial->hide();
    localDials->scaleDial->hide();
    localDials->zoomDial->hide();
    localDials->startDial->hide();
    localDials->sizeDial->hide();

	QSpinBox* offsetDial = new QSpinBox(this);
    offsetDial->setMinimum(-40000000);
    offsetDial->setMaximum(40000000);
    offsetDial->setValue(0);
    offsetDial->setSingleStep(localDials->widthDial->value());
    mainWindow->settingToolBar->addWidget(offsetDial);
    localDials->offsetDial = offsetDial;

	return localDials;
}

void ViewManager::printNum(int num)
{
    ui->print("Global:  ", num);
}

void ViewManager::printNum2(int num)
{
    ui->print("Local:   ", num);
}

//the mother load!
//implement in glWidget
void ViewManager::connectVariables(GLWidget* active, UiVariables* local)
{
    connect(local->sizeDial	, SIGNAL(valueChanged(int)), ui->sizeDial	, SLOT(setValue(int)));
    //connect(ui->sizeDial		, SIGNAL(valueChanged(int)), local->sizeDial	, SLOT(setValue(int)));

    connect(local->widthDial	, SIGNAL(valueChanged(int)), ui->widthDial	, SLOT(setValue(int)));
    connect(ui->widthDial	, SIGNAL(valueChanged(int)), local->widthDial, SLOT(setValue(int)));

    //OLD:widget -> local->start -> pub.start
    //NEW:widget -> local->start -> vMan.slots -> pub.start
    //connect(local->startDial	, SIGNAL(valueChanged(int)), ui->startDial	, SLOT(setValue(int)));
    connect(local->startDial , SIGNAL(valueChanged(int)), this, SLOT(changePublicStart(int)));
    connect(ui->startDial	, SIGNAL(valueChanged(int)),
		dynamic_cast<MdiChildWindow*>(active->parent), SLOT(changeLocalStartFromPublicStart(int)));
    //connect(this, SIGNAL(startChangeFromPublicStart(int)), local->startDial, SLOT(setValue(int)));

    connect(local->scaleDial	, SIGNAL(valueChanged(int)), ui->scaleDial	, SLOT(setValue(int)));
    connect(ui->scaleDial	, SIGNAL(valueChanged(int)), local->scaleDial, SLOT(setValue(int)));

    connect(local->zoomDial	, SIGNAL(valueChanged(int)), ui->zoomDial	, SLOT(setValue(int)));
    connect(ui->zoomDial		, SIGNAL(valueChanged(int)), local->zoomDial	, SLOT(setValue(int)));

    //when a local copy is updated, globals is updated but without this line, the other
    //GLwidgets fail to refresh.
    connect(local, SIGNAL(internalsUpdated()), ui, SIGNAL(internalsUpdated()));
    connect(ui, SIGNAL(internalsUpdated()), active, SLOT(invalidateDisplayGraphs()));
}

void ViewManager::disconnectVariables(GLWidget* active, UiVariables* local)
{
    disconnect(local->sizeDial	, SIGNAL(valueChanged(int)), ui->sizeDial	, SLOT(setValue(int)));
    disconnect(ui->sizeDial		, SIGNAL(valueChanged(int)), local->sizeDial	, SLOT(setValue(int)));

    disconnect(local->widthDial	, SIGNAL(valueChanged(int)), ui->widthDial	, SLOT(setValue(int)));
    disconnect(ui->widthDial	, SIGNAL(valueChanged(int)), local->widthDial, SLOT(setValue(int)));

    disconnect(local->startDial , SIGNAL(valueChanged(int)), this, SLOT(changePublicStart(int)));
    disconnect(ui->startDial	, SIGNAL(valueChanged(int)),
		dynamic_cast<MdiChildWindow*>(active->parent), SLOT(changeLocalStartFromPublicStart(int)));
    //disconnect(this, SIGNAL(startChangeFromPublicStart(int)), local->startDial, SLOT(setValue(int)));
	
    //disconnect(local->offsetDial, SIGNAL(valueChanged(int)), this, SLOT(changeLocalStart(int)));

    disconnect(local->scaleDial	, SIGNAL(valueChanged(int)), ui->scaleDial	, SLOT(setValue(int)));
    disconnect(ui->scaleDial	, SIGNAL(valueChanged(int)), local->scaleDial, SLOT(setValue(int)));

    disconnect(local->zoomDial	, SIGNAL(valueChanged(int)), ui->zoomDial	, SLOT(setValue(int)));
    disconnect(ui->zoomDial		, SIGNAL(valueChanged(int)), local->zoomDial	, SLOT(setValue(int)));

    //when a local copy is updated, globals is updated but without this line, the other
    //GLwidgets fail to refresh.
    disconnect(local, SIGNAL(internalsUpdated()), ui, SIGNAL(internalsUpdated()));
    disconnect(ui, SIGNAL(internalsUpdated()), active, SLOT(invalidateDisplayGraphs()));
}

UiVariables* ViewManager::vars(GLWidget* active)
{
	return dynamic_cast<MdiChildWindow*>(active->parent)->ui;
}

void ViewManager::updateCurrentDisplay(){
    if (mainWindow->syncCheckBox->isChecked())
    {
        for(int i = 0; i < (int)views.size(); ++i)
            views[i]->glWidget->invalidateDisplayGraphs();
    }
	else {
        activeWidget->invalidateDisplayGraphs();
	}
}
