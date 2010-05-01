#include "MdiChildWindow.h"
#include "glwidget.h"
#include <QtGui/QTabWidget>
#include "HighlightDisplay.h"  //TODO: remove dependency

MdiChildWindow::MdiChildWindow(UiVariables gui, QSpinBox* pStart, QTabWidget* settings)
{
	ui = gui;
	publicStart = pStart;
	settingsDock = settings;
	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );

	subFrame = new QFrame(this);
	glWidget = new GLWidget(ui, this);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(glWidget);
	hLayout->addWidget(verticalScrollBar);
	subFrame->setLayout(hLayout);//
	
	setWindowTitle("Skittle Logo");
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(subFrame);
	vLayout->addWidget(horizontalScrollBar);
	setLayout(vLayout);
	
    //scrollArea->setWidget(glWidget);//glWidget);
    setAttribute(Qt::WA_DeleteOnClose);
    
	/**Scrollbars**/
	connect(glWidget, SIGNAL(displaySizeChanged()), this, SLOT(setPageSize()) );
	//connect(ui.sizeDial, SIGNAL(valueChanged(int)), this, SLOT(setPageSize()) );	
	connect(verticalScrollBar, SIGNAL(valueChanged(int)), ui.startDial, SLOT(setValue(int)));
	connect(ui.startDial, SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));
	/**/
	connectWidget();
	setPageSize();
	createSettingsTabs();
}	

void MdiChildWindow::changeLocalStartFromPublicStart(int val)
{
	int start = val;
	int offset = ui.offsetDial->value();
	
	ui.startDial->setValue((int)max(0, start + offset));
	//emit startChangeFromPublicStart((int)max(0, start + offset));
}

void MdiChildWindow::changeLocalStartFromOffset(int val)
{
	int start = publicStart->value();
	int offset = ui.offsetDial->value();
	ui.startDial->setValue((int)max(0, start + offset));
}

void MdiChildWindow::closeEvent(QCloseEvent *event)
{
	//QScrollBar* horizontalScrollBar;
	//QScrollBar* verticalScrollBar;
	//QFrame* subFrame;
	delete glWidget;	
	delete ui.offsetDial;//->hide();
	for(int i = 0; i < (int)settingsTabs.size(); ++i)
		delete settingsTabs[i];
	event->accept();
}

void MdiChildWindow::connectWidget()
{
	connect(horizontalScrollBar, SIGNAL(valueChanged(int)), glWidget, SLOT(slideHorizontal(int)));
	connect(glWidget, SIGNAL(xOffsetChange(int)), horizontalScrollBar, SLOT(setValue(int)));	
	connect(glWidget, SIGNAL(totalWidthChanged(int)), this, SLOT(setHorizontalWidth(int)));
	connect(ui.widthDial, SIGNAL(valueChanged(int)), this, SLOT(setOffsetStep(int)));
	connect(ui.offsetDial, SIGNAL(valueChanged(int)), this, SLOT(changeLocalStartFromOffset(int)));
}

void MdiChildWindow::setHorizontalWidth(int val)
{
	horizontalScrollBar->setMaximum( max(0, val) );
}

void MdiChildWindow::setOffsetStep(int val)
{
	ui.offsetDial->setSingleStep(val);
}

void MdiChildWindow::setPageSize()
{
	//glWidget->print("setPageSize", ui.sizeDial->value());
	if( glWidget != NULL)
		verticalScrollBar->setMaximum( max(0, (int)(glWidget->seq()->size() - ui.widthDial->value()) ) );
	verticalScrollBar->setPageStep(ui.sizeDial->value());
}

void MdiChildWindow::createSettingsTabs()
{
	//QFrame* tab = glWidget->highlight->settingsUi();
	settingsTabs = glWidget->settingsUi();
	for(int i = 0; i < (int)settingsTabs.size(); ++i)
	{
		if( settingsTabs[i] != NULL )
			settingsDock->addTab(settingsTabs[i], settingsTabs[i]->windowTitle());
		else
			glWidget->print("WARNING: NULL entry in MdiChildWindow::settingsTabs");
	}
}

void MdiChildWindow::hideSettingsTabs()
{
	for(int i = 0; i < (int)settingsTabs.size(); ++i)
	{
		int index = settingsDock->indexOf(settingsTabs[i]);
		if(index > -1)
			settingsDock->removeTab(index);
		//settingsTabs[i]->removeTab(1);
	}
}

void MdiChildWindow::showSettingsTabs()
{
	for(int i = 0; i < (int)settingsTabs.size(); ++i)
	{
		settingsDock->insertTab(1, settingsTabs[i], settingsTabs[i]->windowTitle());
		//settingsTabs[i]->insertTab(1);
	}
}

