//ViewSplitter.cpp
#include "ViewManager.h"
#include "glwidget.h"
#include "GtfReader.h"
#include <QtGui/QScrollBar>

ViewManager::ViewManager(UiVariables gui)
{
	ui = gui;
	activeWidget = NULL;
	connect(ui.sizeDial, SIGNAL(valueChanged(int)), this, SLOT(setPageSize()) );

	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );
	
		//scrollArea = new QFrame;
	setBackgroundRole(QPalette::Dark);
	//	glWidgets = new vector<GLWidget*>;
	glWidget = new GLWidget(ui, this);
	glWidget2 = new GLWidget(ui, this);
	
	connect(glWidget, SIGNAL(IveBeenClicked(GLWidget*)), this, SLOT(changeSelection(GLWidget*)));
	connect(glWidget2, SIGNAL(IveBeenClicked(GLWidget*)), this, SLOT(changeSelection(GLWidget*)));
	
	changeSelection(glWidget);

	QFrame* subFrame = new QFrame;
	
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(glWidget);
	hLayout->addWidget(glWidget2);
	hLayout->addWidget(verticalScrollBar);
	subFrame->setLayout(hLayout);
	
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(subFrame);
	vLayout->addWidget(horizontalScrollBar);
	setLayout(vLayout);
	
	connect(verticalScrollBar, SIGNAL(valueChanged(int)), ui.startDial, SLOT(setValue(int)));
	connect(ui.startDial, SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));

}
//public slots
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
