#include "AbstractGraph.h"
#include "UiVariables.h"
#include "glwidget.h"
#include <math.h>

AbstractGraph::AbstractGraph()
{
	textureBuffer = new TextureCanvas();
}

AbstractGraph::~AbstractGraph()
{
	if(toggleButton != NULL)
		emit deleteButton(toggleButton);
}
/****************************************
 * NOTE: Unused code:
 * 
 * At Present, Nothing seems to be calling 
 * this. There is a commented call in 
 * GLWidget::createConnections(), but
 * that is the only way that it is being 
 * called.  
 ***************************************/
void AbstractGraph::createConnections() 
{
	connect( this, SIGNAL(widthChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(startChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(sizeChanged(int)), this, SIGNAL(displayChanged()));
	
	connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
	connect( this, SIGNAL(widthChanged(int)), ui->widthDial, SLOT(setValue(int)));
	
	connect( ui->startDial, SIGNAL(valueChanged(int)), this, SLOT(changeStart(int)));
	connect( this, SIGNAL(startChanged(int)), ui->startDial, SLOT(setValue(int)));
	
	connect( ui->sizeDial, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect( this, SIGNAL(sizeChanged(int)), ui->sizeDial, SLOT(setValue(int)));

	connect( ui->scaleDial, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
	connect( this, SIGNAL(scaleChanged(int)), ui->scaleDial, SLOT(setValue(int)));
	//NOTE: the above line may resolve a synchronization loop issue
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

void AbstractGraph::storeDisplay(vector<color>& pixels, int width)
{
	if(textureBuffer)
		delete textureBuffer;
	textureBuffer = new TextureCanvas( pixels, width );
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
	changeSize(ui->sizeDial->value());
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
