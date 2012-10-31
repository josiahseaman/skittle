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

Changes to the dials are connected to each window and propagate to display updates.
Offsets must be handled as a special condition.  Look at addNewView() to see what happens
when a new player comes on stage.

Window Hierarchy:
MainWindow -> (1) Viewmanager -> (many) MdiChildWindow -> (1) GLWidget -> (1)FastaReader -> (1)File
MainWindow is at the top of a window hierarchy.  The center widget of MainWindow is a single
ViewManager which inherits from MdiArea.  The multiple document interface (MDI) can have multiple
MdiChildWindows.  Each MdiChildWindow has only one GLWidget tied to one file.
UiVariables is a singleton and can be accessed anywhere by calling UiVariables::Instance().  It
is responsible for maintaining state all throughout the program.
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
    if(activeWidget)
        activeWidget->showMaximized();//if there's only one window, I'd like it maximized
}

void ViewManager::createConnections()
{
    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(changeSelection(QMdiSubWindow*)));
    connect(mainWindow->addViewAction, SIGNAL(triggered()), this, SLOT(addNewView()));
    /****CONNECT ui VARIABLES*******/

    connect(ui->widthDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->zoomDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->scaleDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->startDial, SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
    connect(ui->sizeDial,  SIGNAL(editingFinished()), this, SLOT(updateCurrentDisplay()));
}

void ViewManager::uiToGlwidgetConnections(GLWidget* active)
{
//    connect(active->parent, SIGNAL(IveBeenClicked(MdiChildWindow*)), this, SLOT(changeSelection(MdiChildWindow*)));
    connect(mainWindow->moveAction,          SIGNAL(triggered()), active, SLOT(on_moveButton_clicked()));
    connect(mainWindow->selectAction,        SIGNAL(triggered()), active, SLOT(on_selectButton_clicked()));
    connect(mainWindow->findAction,          SIGNAL(triggered()), active, SLOT(on_findButton_clicked()));
    connect(mainWindow->screenCaptureAction, SIGNAL(triggered()), active, SLOT(on_screenCaptureButton_clicked()));
    connect(mainWindow->resizeAction,        SIGNAL(triggered()), active, SLOT(on_resizeButton_clicked()));
    connect(mainWindow->zoomAction,          SIGNAL(triggered()), active, SLOT(on_zoomButton_clicked()));
    connect(mainWindow->addAnnotationAction, SIGNAL(triggered()), active, SLOT(on_addAnnotationButton_clicked()));
    connect(mainWindow->zoomExtents,         SIGNAL(clicked()),   active, SLOT(zoomExtents()));

    connect( active, SIGNAL(addGraphMode(AbstractGraph*)), mainWindow, SLOT(addDisplayActions(AbstractGraph*)));
    connect( active, SIGNAL(addDivider()), mainWindow, SLOT(addDisplayDivider()));
    active->createButtons();
}

//public slots
GLWidget* ViewManager::addNewView(bool suppressOpen)
{
    if(activeWidget)
        activeWidget->showNormal();//this should be the equivalent of clicking 'restore' or not maximized
    int offsetIndex = newOffsetDial();

    MdiChildWindow* child = new MdiChildWindow(offsetIndex, mainWindow->tabWidget);
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

void ViewManager::changeSelection(QMdiSubWindow* container)
{
    if(container == NULL)
        return;
    const QObjectList children = container->children();
    MdiChildWindow* newActiveMdi = NULL;
    for(int i = 0; newActiveMdi == NULL && i < (int)children.size(); ++i )//look through children for an MdiChildWindow
        newActiveMdi = dynamic_cast<MdiChildWindow*>(children.at(i));

    if(newActiveMdi != NULL)
        changeSelection(newActiveMdi);
}

void ViewManager::changeSelection(MdiChildWindow* newActiveMdi)
{
    if(newActiveMdi == NULL || newActiveMdi->glWidget == activeWidget)
        return;

    GLWidget* oldActiveGlWidget = activeWidget;

    int tabIndex = mainWindow->tabWidget->currentIndex();
    if(oldActiveGlWidget != NULL)//deal with oldActiveGlWidget
    {
        oldActiveGlWidget->parent->hideSettingsTabs();
    }
    newActiveMdi->showSettingsTabs();

    mainWindow->tabWidget->setCurrentIndex(tabIndex);
    activeWidget = newActiveMdi->glWidget ;
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

        //Attempt to load the skittle notes file for this file if it exists
        if (QFile(fileName + "-skittle_notes.gff").exists())
        {
            addAnnotationDisplay(fileName + "-skittle_notes.gff");
        }
    }
}

void ViewManager::addAnnotationDisplay(QString fileName)
{
    if(activeWidget != NULL)
    {
        activeWidget->addAnnotationDisplay(fileName);
    }
}

void ViewManager::jumpToNextAnnotation()
{
    if(activeWidget != NULL)
    {
        activeWidget->jumpToAnnotation(true);
    }
}
void ViewManager::jumpToPrevAnnotation()
{
    if(activeWidget != NULL)
    {
        activeWidget->jumpToAnnotation(false);
    }
}

void ViewManager::changeGlobalStart()
{
//    UiVariables* local = vars(activeWidget);
    int preOffsetStart = max(1, ui->startDial->value() - ui->getOffsetDial(0)->value());//TODO: use widget offsetIndex
    ui->changeStart(preOffsetStart);
}

//PRIVATE FUNCTIONS//
int ViewManager::newOffsetDial()
{
    int offsetIndex = ui->newOffsetDial();
    QSpinBox* offset = ui->getOffsetDial(offsetIndex);
    if(offset)
    {
        offset->show();
        mainWindow->settingToolBar->addWidget(offset);
    }
    return offsetIndex;
}

UiVariables* ViewManager::vars(GLWidget* active)
{
    return dynamic_cast<MdiChildWindow*>(active->parent)->ui;
}

void ViewManager::updateCurrentDisplay()
{
    for(int i = 0; i < (int)views.size(); ++i)
        views[i]->glWidget->invalidateDisplayGraphs();
}
