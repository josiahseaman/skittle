#include "Controller.h"
#include "window.h"
#include "FastaReader.h"
#include "GtfReader.h"
#include "BasicTypes.h"
#include <sstream>

//A simple connection point for all QObjects to communicate
Controller::Controller(UiVariables* win)
{
	scale = 1;
	window = win;
	ui = win;//new MainWindow();
	//ui->setupUi(win);
	
    glWidget = ui->glWidget;


}

