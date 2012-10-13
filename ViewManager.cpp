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
    globalUi = gui;
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
	
    connect(globalUi->widthDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(globalUi->zoomDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(globalUi->scaleDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(globalUi->startDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(globalUi->sizeDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
}

void ViewManager::uiToGlwidgetConnections(GLWidget* active)
{
    connect(active->parent, SIGNAL(IveBeenClicked(MdiChildWindow*)), this, SLOT(changeSelection(MdiChildWindow*)));
    connect(mainWindow->moveAction, SIGNAL(triggered()), active, SLOT(on_moveButton_clicked()));
    connect(mainWindow->selectAction, SIGNAL(triggered()), active, SLOT(on_selectButton_clicked()));
    connect(mainWindow->findAction, SIGNAL(triggered()), active, SLOT(on_findButton_clicked()));
    connect(mainWindow->resizeAction, SIGNAL(triggered()), active, SLOT(on_resizeButton_clicked()));
    connect(mainWindow->zoomAction, SIGNAL(triggered()), active, SLOT(on_zoomButton_clicked()));
    connect(mainWindow->zoomExtents, SIGNAL(clicked()), active, SLOT(zoomExtents()));
	
	connect( active, SIGNAL(addGraphMode(AbstractGraph*)), mainWindow, SLOT(addDisplayActions(AbstractGraph*)));
	
	connect( active, SIGNAL(addDivider()), mainWindow, SLOT(addDisplayDivider()));
	active->createButtons();

}

//public slots
GLWidget* ViewManager::addNewView(bool suppressOpen)
{
    UiVariables* localDials = copyUi();
	broadcastPublicValues(localDials);
	
    MdiChildWindow* child = new MdiChildWindow(localDials, globalUi->startDial, mainWindow->tabWidget);//TODO: figure out a better way to manage startDial
	connect( child, SIGNAL(subWindowClosing(MdiChildWindow*)), this, SLOT(closeSubWindow(MdiChildWindow*)));
    addSubWindow(child);
    child->show();
    views.push_back(child);
	
    GLWidget* newGlWidget = child->glWidget;
	uiToGlwidgetConnections(newGlWidget);
	changeSelection(child);

	if(suppressOpen == false)
		mainWindow->openAction->trigger();
	return newGlWidget;
}

void ViewManager::changeSelection(MdiChildWindow* newActiveMdi)
{
    GLWidget* newActiveGlWidget = newActiveMdi->glWidget;
    GLWidget* oldActiveGlWidget = activeWidget;
    if(newActiveGlWidget == oldActiveGlWidget)
		return;
		
	int tabIndex = mainWindow->tabWidget->currentIndex();
    if(oldActiveGlWidget != NULL)//deal with oldActiveGlWidget
	{
        oldActiveGlWidget->parent->hideSettingsTabs();
        disconnectLocalPushToGlobal(oldActiveGlWidget, vars(oldActiveGlWidget));
        if(mainWindow->syncCheckBox->isChecked() == false )
            disconnectGlobalPushToLocal(oldActiveGlWidget, vars(oldActiveGlWidget));
	}
    //deal with newActiveGlWidget
    connectGlobalPushToLocal(newActiveGlWidget, vars(newActiveGlWidget));//this will not create duplicate connections
    connectLocalPushToGlobal(newActiveGlWidget, vars(newActiveGlWidget));
    newActiveMdi->showSettingsTabs();

	mainWindow->tabWidget->setCurrentIndex(tabIndex);	
    activeWidget = newActiveGlWidget;
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

void ViewManager::jumpToNextAnnotation()
{
    if(activeWidget != NULL)
    {
        activeWidget->jumpToNextAnnotation();
    }
}

void ViewManager::handleWindowSync()
{
	if( mainWindow->syncCheckBox->isChecked() )//reconnect all windows
	{
		for(int i = 0; i < (int)views.size(); ++i)
        {
            connectGlobalPushToLocal(views[i]->glWidget, views[i]->ui);
		}
	}
	else//disconnect all windows but the current one
	{
		for(int i = 0; i < (int)views.size(); ++i)
		{
			if(views[i]->glWidget != activeWidget)
                disconnectGlobalPushToLocal(views[i]->glWidget, views[i]->ui);
		}
	}
}

void ViewManager::changeGlobalStart()
{
    UiVariables* local = vars(activeWidget);
    int preOffsetStart = max(1, local->startDial->value() - local->offsetDial->value());
    globalUi->startDial->setValue(preOffsetStart);
//    globalUi->changeStart(preOffsetStart);
}

void ViewManager::changeAllLocalStarts()
{
    for(int i = 0; i < (int)views.size(); ++i)
    {
        UiVariables* local = views[i]->ui;
        local->changeStart(globalUi->startDial->value() + local->offsetDial->value());
    }
}

//PRIVATE FUNCTIONS//
void ViewManager::broadcastPublicValues(UiVariables* local)
{
    local->startDial->setValue(globalUi->startDial->value());
    local->sizeDial->setValue(globalUi->sizeDial->value());
    local->widthDial->setValue(globalUi->widthDial->value());
    local->scaleDial->setValue(globalUi->scaleDial->value());
    local->zoomDial->setValue(globalUi->zoomDial->value());
}

UiVariables* ViewManager::copyUi()
{
    UiVariables* localDials = new UiVariables(globalUi->textArea);
    localDials->widthDial->hide();
    localDials->scaleDial->hide();
    localDials->zoomDial->hide();
    localDials->startDial->hide();
    localDials->sizeDial->hide();

    localDials->offsetDial->show();
    mainWindow->settingToolBar->addWidget(localDials->offsetDial);

	return localDials;
}

void ViewManager::printNum(int num)
{
    globalUi->print("Global:  ", num);
}

void ViewManager::printNum2(int num)
{
    globalUi->print("Local:   ", num);
}

void ViewManager::connectGlobalPushToLocal(GLWidget* active, UiVariables* local) //all windows are listening with syncCheckbox
{
    /** We are using Qt::UniqueConnection just to make sure we aren't forming duplicate connections */
    //connect(globalUi->sizeDial, SIGNAL(valueChanged(int)), local->sizeDial   , SLOT(setValue(int)));
    connect(globalUi->widthDial	, SIGNAL(valueChanged(int)), local->widthDial  , SLOT(setValue(int)), Qt::UniqueConnection);
    connect(globalUi->startDial	, SIGNAL(valueChanged(int)), this              , SLOT(changeAllLocalStarts()), Qt::UniqueConnection);
    connect(globalUi->scaleDial	, SIGNAL(valueChanged(int)), local->scaleDial  , SLOT(setValue(int)), Qt::UniqueConnection);
    connect(globalUi->zoomDial  , SIGNAL(valueChanged(int)), local->zoomDial   , SLOT(setValue(int)), Qt::UniqueConnection);
    connect(globalUi, SIGNAL(internalsUpdated()), active, SLOT(invalidateDisplayGraphs()), Qt::UniqueConnection);
    connect(globalUi, SIGNAL(colorsChanged(int)), local, SLOT(changeColorSetting(int)), Qt::UniqueConnection); // Should never be disconnected
}

void ViewManager::connectLocalPushToGlobal(GLWidget* active, UiVariables* local) //only active window gets to push to globalUi
{
    connect(local->sizeDial	 , SIGNAL(valueChanged(int)), globalUi->sizeDial	, SLOT(setValue(int)));
    connect(local->widthDial , SIGNAL(valueChanged(int)), globalUi->widthDial	, SLOT(setValue(int)));
    //connect(local->offsetDial, SIGNAL(valueChanged(int)), active->parent, SLOT(changeLocalStartFromOffset(int)));
    connect(local->startDial , SIGNAL(valueChanged(int)), this, SLOT(changeGlobalStart()));//NEW:widget -> local->start -> vMan.slots -> pub.start
    connect(local->scaleDial , SIGNAL(valueChanged(int)), globalUi->scaleDial	, SLOT(setValue(int)));
    connect(local->zoomDial	 , SIGNAL(valueChanged(int)), globalUi->zoomDial	, SLOT(setValue(int)));

    //when a local copy is updated, globals is updated but without this line, the other
    //GLwidgets fail to refresh.
    connect(local, SIGNAL(internalsUpdated()), globalUi, SIGNAL(internalsUpdated()));
}

void ViewManager::disconnectGlobalPushToLocal(GLWidget* active, UiVariables* local)
{
    disconnect(globalUi->sizeDial		, SIGNAL(valueChanged(int)), local->sizeDial	, SLOT(setValue(int)));
    disconnect(globalUi->widthDial	, SIGNAL(valueChanged(int)), local->widthDial, SLOT(setValue(int)));
    disconnect(globalUi->startDial	, SIGNAL(valueChanged(int)), this, SLOT(changeAllLocalStarts()));
    disconnect(globalUi->scaleDial	, SIGNAL(valueChanged(int)), local->scaleDial, SLOT(setValue(int)));
    disconnect(globalUi->zoomDial		, SIGNAL(valueChanged(int)), local->zoomDial	, SLOT(setValue(int)));
    disconnect(globalUi, SIGNAL(internalsUpdated()), active, SLOT(invalidateDisplayGraphs()));
}

void ViewManager::disconnectLocalPushToGlobal(GLWidget* active, UiVariables* local)
{
    disconnect(local->sizeDial	, SIGNAL(valueChanged(int)), globalUi->sizeDial	, SLOT(setValue(int)));
    disconnect(local->widthDial	, SIGNAL(valueChanged(int)), globalUi->widthDial	, SLOT(setValue(int)));
    disconnect(local->startDial , SIGNAL(valueChanged(int)), this, SLOT(changeGlobalStart()));
    disconnect(local->scaleDial	, SIGNAL(valueChanged(int)), globalUi->scaleDial	, SLOT(setValue(int)));
    disconnect(local->zoomDial	, SIGNAL(valueChanged(int)), globalUi->zoomDial	, SLOT(setValue(int)));
    disconnect(local, SIGNAL(internalsUpdated()), globalUi, SIGNAL(internalsUpdated()));

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
