#include "AbstractGraph.h"
#include "UiVariables.h"
#include "glwidget.h"
#include <math.h>


/** *****************
  Abstract Graph is the abstract class that all of the Graphs inherit from.
  Graph are the main workhorses of Skittle.  The are different ways of
  displaying the genome sequence.  This class should contain all of the
  universal functionality of the graphs like receiving data, sending signals,
  handling variable changes and painting using a TextureCanvas.  The constructor requires
*  a Ui reference so that it can 'connect' to controls in the Ui.
  */
AbstractGraph::AbstractGraph()
{
    usingTextures = true;
	textureBuffer = new TextureCanvas();
}

AbstractGraph::~AbstractGraph()
{
	if(toggleButton != NULL)
		emit deleteButton(toggleButton);
}

void AbstractGraph::checkVariables()
{
	changeScale(ui->scaleDial->value());
	changeWidth(ui->widthDial->value());
	changeStart(ui->startDial->value());
	changeSize(ui->sizeDial->value());
}

int AbstractGraph::height()
{
	return display_size / Width;
}

void AbstractGraph::paint_square(point position, color c)
{	
	glPushMatrix();
    	glColor3d(c.r /255.0, c.g /255.0, c.b /255.0); 
        glTranslated(position.x+1, position.y, position.z);
        glBegin(GL_QUADS);
	        glVertex3d(.0, .0, 0);
	        glVertex3d(-1.0, .0, 0);
	        glVertex3d(-1, -1, 0);
	        glVertex3d(.0, -1, 0);
	    glEnd();
	glPopMatrix();
}

void AbstractGraph::storeDisplay(vector<color>& pixels, int width, bool raggedEdge)
{
	if(textureBuffer)
		delete textureBuffer;
    textureBuffer = new TextureCanvas( pixels, width, raggedEdge );
}

bool AbstractGraph::updateInt(int& subject, int& value)
{
	if(value < 1)
		value = 1;
	if(subject != value)
	{
		subject = value;	
		upToDate = false;
		return true;
	}	
	return false;
}

bool AbstractGraph::updateVal(double& subject, double& value)
{
	if(value < 0.01)
		value = 0.01;
	if(subject != value)
	{
		subject = value;	
		upToDate = false;
		return true;
	}	
	return false;
}

void AbstractGraph::displayLegend(float canvasWidth, float canvasHeight)
{
}


string AbstractGraph::getFileName()
{
	return "";
}

//***********SLOTS*******************
void AbstractGraph::changeScale(int s)
{
	if(updateInt(scale, s))
	{
		emit scaleChanged(s);
	}
}
	
void AbstractGraph::changeWidth(int w)
{
	if(updateInt(Width, w ))
	{
		if(Width < scale)
			Width = scale;
		emit widthChanged(Width);
	}
}

void AbstractGraph::changeSize(int s)
{
	if(updateInt(max_display_size, s))
	{
		display_size = min( max_display_size, max(0, ((int)sequence->size() - nucleotide_start)) );
		emit sizeChanged(s);
	}
}

void AbstractGraph::changeStart(int s)
{
	if(updateInt(nucleotide_start, s))
	{
		display_size = min( max_display_size, max(0, ((int)sequence->size() - nucleotide_start)) );
		emit startChanged(s);
	}
}

void AbstractGraph::invalidate()
{
	upToDate = false;
	emit displayChanged();
}

void AbstractGraph::ensureVisible()
{
	if(hidden)
		toggleVisibility();
}

void AbstractGraph::toggleVisibility()
{
	hidden = !hidden;
	if(settingsTab != NULL)
	{
		if(hidden)
			emit hideSettings(settingsTab);
		else
			emit showSettings(settingsTab);
	}	
	setButtonFont();
	emit displayChanged();
}

void AbstractGraph::setButtonFont()
{
	if(toggleButton != NULL)
	{
		QFont font = toggleButton->font();
		if(hidden){
			font.setBold(false);
		}
		else{
			font.setBold(true);
		}
		toggleButton->setFont(font);
	}
}

void AbstractGraph::setSequence(const string* seq)
{
    sequence = seq;
    changeSize(ui->sizeDial->value());//TODO: is this accomplishing anything on a new file read?
}

string AbstractGraph::mouseClick(point2D pt)
{	
	return string();
}

int AbstractGraph::width() 
{
	return Width / scale;
}

QScrollArea* AbstractGraph::settingsUi()
{//settingsUi should be called before toggleVisibility() the first time so that "settingsTab" will have a value
	settingsTab = NULL;
	return NULL;
}

string AbstractGraph::reverseComplement(string original)
{
	string rc;
	int size = original.size();
	rc.resize(size, 'N');
	for(int x = 0; x < size; ++x)
	{					
		rc[x] = complement(original[size-x-1]);
	}
	return rc;
}
