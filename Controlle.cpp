#include "Controller.h"
#include "window.h"
#include "FastaReader.h"
#include "GtfReader.h"
#include "BasicTypes.h"
#include <sstream>

//A simple connection point for all QObjects to communicate
Controller::Controller(Window* win)
{
	scale = 1;
	window = win;
	ui = new Ui_SkittleGUI();
	ui->setupUi(win);
	
    glWidget = ui->glWidget;
    fastaReader = new FastaReader(ui);
    trackReader = new GtfReader(ui);

    connect(ui->actionOpen, SIGNAL(triggered()), window, SLOT(open()));
    connect(window, SIGNAL(newFileOpen(QString)), fastaReader, SLOT(readFile(QString)));
    connect(fastaReader, SIGNAL(newFileRead(const string&)), glWidget, SLOT(displayString(const string&)));
    connect(fastaReader, SIGNAL(fileNameChanged(string)), win, SLOT(changeWindowName(string)));
    
    connect(window, SIGNAL(newFileOpen(QString)), trackReader, SLOT(determineOutputFile(QString)));

	connect(ui->actionOpenGTF, SIGNAL(triggered()), window, SLOT(openGtf()));
	connect(window, SIGNAL(newGtfFileOpen(QString)), trackReader, SLOT(readFile(QString)));
	connect(trackReader, SIGNAL(newGtfFileRead(const vector<track_entry>&)), glWidget, SLOT(newAnnotation(const vector<track_entry>&)));
	connect(ui->bookmarkButton, SIGNAL(clicked()), trackReader, SLOT(addBookmark()));

    connect(ui->moveButton, SIGNAL(clicked()), glWidget, SLOT(on_moveButton_clicked()));
    connect(ui->selectButton, SIGNAL(clicked()), glWidget, SLOT(on_selectButton_clicked()));
    connect(ui->resizeButton, SIGNAL(clicked()), glWidget, SLOT(on_resizeButton_clicked()));
    
	
	connect( ui->scaleDial, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
	
	connect( ui->widthX2Button, SIGNAL(clicked()), this, SLOT(doubleWidth()));
	connect( ui->widthDiv2Button, SIGNAL(clicked()), this, SLOT(halveWidth()));
}

void Controller::changeScale(int newScale)
{	
	if(newScale < 1)
		newScale = 1;
	if(scale != newScale)
	{
		ui->widthDial->setSingleStep(newScale);
		//if(!ui->widthLock->isChecked())
		{
			int display_width = ui->widthDial->value() / scale;
			display_width = max( 1, display_width);
			int display_size = ui->sizeDial->value() / scale;
			display_size = max( 1, display_size);
			scale = newScale;
			ui->widthDial->setValue( display_width * newScale );
		}
	}
}


void Controller::doubleWidth()
{
		ui->widthDial->setValue( 2 * ui->widthDial->value() );
}

void Controller::halveWidth()
{
		ui->widthDial->setValue( (int)(0.5 * ui->widthDial->value()) );
}
