#include "MdiChildWindow.h"
#include "glwidget.h"

MdiChildWindow::MdiChildWindow(UiVariables gui)
{
	ui = gui;
	horizontalScrollBar = new QScrollBar();
	horizontalScrollBar->setOrientation(Qt::Horizontal);
    horizontalScrollBar->setMaximum (50);
    horizontalScrollBar->setPageStep(100);
    horizontalScrollBar->setSingleStep(10);
	verticalScrollBar = new QScrollBar();
	verticalScrollBar->setMaximum( 100 );

	subFrame = new QFrame(this);
	glWidget = new GLWidget(ui, subFrame);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(glWidget);
	hLayout->addWidget(verticalScrollBar);
	subFrame->setLayout(hLayout);//
	
	//largeFrame = new QFrame(this);
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

}	

void MdiChildWindow::connectWidget()
{
	connect(horizontalScrollBar, SIGNAL(valueChanged(int)), glWidget, SLOT(slideHorizontal(int)));
	connect(glWidget, SIGNAL(xOffsetChange(int)), horizontalScrollBar, SLOT(setValue(int)));	
	//connect resizing horizontalScroll
	connect(glWidget, SIGNAL(totalWidthChanged(int)), this, SLOT(setHorizontalWidth(int)));
}

void MdiChildWindow::setHorizontalWidth(int val)
{
	horizontalScrollBar->setMaximum( max(0, val) );
}

void MdiChildWindow::setPageSize()
{
	//glWidget->print("setPageSize", ui.sizeDial->value());
	if( glWidget != NULL)
		verticalScrollBar->setMaximum( max(0, (int)(glWidget->seq()->size() - ui.widthDial->value()) ) );
	verticalScrollBar->setPageStep(ui.sizeDial->value());
}
