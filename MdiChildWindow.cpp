#include "MdiChildWindow.h"
#include "glwidget.h"
#include <QtGui/QTabWidget>
#include "HighlightDisplay.h"  //TODO: remove dependency
#include <algorithm>

using std::find;

MdiChildWindow::MdiChildWindow(UiVariables gui, QSpinBox* pStart, QTabWidget* settings)
	: ui(gui)
{
	publicStart = pStart;
	settingsDock = settings;
	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );
	
	setMouseTracking(true);
    //setFocusPolicy(Qt::ClickFocus);
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
	connect(verticalScrollBar, SIGNAL(sliderReleased()), glWidget, SLOT(updateDisplay()));
	connect(ui.startDial, SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));
	/**/
	createSettingsTabs();
	connectWidget();
	setPageSize();
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
	emit subWindowClosing(this);
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
	//ui.print("setPageSize", ui.sizeDial->value());
	if( glWidget != NULL)
		verticalScrollBar->setMaximum( max(0, (int)(glWidget->seq()->size() - ui.widthDial->value()) ) );
	verticalScrollBar->setPageStep(ui.sizeDial->value());
}

void MdiChildWindow::createSettingsTabs()
{
	//QFrame* tab = glWidget->highlight->settingsUi();
	glWidget->settingsUi();//settingsTabs = 
	/*for(int i = 0; i < (int)temp.size(); ++i)
	{
		if( settingsTabs[i] == NULL )
			ui.print("WARNING: NULL entry in MdiChildWindow::settingsTabs");
		//else
			//settingsDock->addTab(settingsTabs[i], settingsTabs[i]->windowTitle());
			
	}*/
	connect( glWidget, SIGNAL(hideSettings(QScrollArea*)), this, SLOT(hideSettingsTab(QScrollArea*)));
	connect( glWidget, SIGNAL(showSettings(QScrollArea*)), this, SLOT(showSettingsTab(QScrollArea*)));
}

//NOTE: The plural and singular forms of (hide/show)SettingsTab(s) are similar but not identical in where they get the pointer
void MdiChildWindow::hideSettingsTab(QScrollArea* tab)
{
	int index = settingsDock->indexOf(tab);
	if(index > -1)
	{
		settingsDock->removeTab(index); 
		vector<QScrollArea*>::iterator it;
		it = std::find(settingsTabs.begin(), settingsTabs.end(), tab);
		settingsTabs.erase(it);
	}
}
void MdiChildWindow::showSettingsTab(QScrollArea* tab)
{
	settingsTabs.push_back(tab);
	settingsDock->insertTab(1, tab, tab->windowTitle());
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

void MdiChildWindow::mousePressEvent(QMouseEvent *event)
{
	emit IveBeenClicked(this);
	QFrame::mousePressEvent(event);
}

