//ViewSplitter.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "GtfReader.h"

ViewManager::ViewManager(UiVariables gui)
{
	ui = gui;
	activeWidget = NULL;
	connect(ui.sizeDial, SIGNAL(valueChanged(int)), this, SLOT(setPageSize()) );
}
//public slots
void ViewManager::changeSelection(GLWidget* current)
{
	if(activeWidget != NULL)
		disconnectWidget();
	
	activeWidget = current;
	setPageSize();
	connectWidget();
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
		ui.verticalScrollBar->setMaximum( max(0, (int)(activeWidget->seq()->size() - ui.widthDial->value()) ) );
	ui.verticalScrollBar->setPageStep(ui.sizeDial->value());
}

void ViewManager::connectWidget()
{		    	
	connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), activeWidget, SLOT(slideHorizontal(int)));
	connect(activeWidget, SIGNAL(xOffsetChange(int)), ui.horizontalScrollBar, SLOT(setValue(int)));	
}

void ViewManager::disconnectWidget()
{
	disconnect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), activeWidget, SLOT(slideHorizontal(int)));
	disconnect(activeWidget, SIGNAL(xOffsetChange(int)), ui.horizontalScrollBar, SLOT(setValue(int)));		
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
