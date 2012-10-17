#include "AbstractGraph.h"
#include "UiVariables.h"
#include "glwidget.h"
#include <sstream>
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

AbstractGraph::AbstractGraph(UiVariables* gui, GLWidget* gl)
{
    glWidget = gl;
    ui = gui;
    string* seq = new string("AATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATT");
    sequence = seq;
    hidden = true;
    settingsTab = NULL;
    toggleButton = NULL;
    textureBuffer = new TextureCanvas();

    usingTextures = true;
    frameCount = 0;
    display_object = 0;
    upToDate = false;
}

AbstractGraph::~AbstractGraph()
{
	if(toggleButton != NULL)
		emit deleteButton(toggleButton);

    glDeleteLists(display_object, 1);//for the Graphs that use a cached GL_Display_List,
    //you need to delete it on exit so it doesn't suck up memory on the graphics card.
}

void AbstractGraph::checkVariables()
{
//This is a place holder for the case where a Graph needs to check its settings tab
}

void AbstractGraph::display()
{
    checkVariables();
    glPushMatrix();
    glScaled(1,-1,1);
        if(!upToDate)
            calculateOutputPixels();
        textureBuffer->display();
    glPopMatrix();
}

GLuint AbstractGraph::render()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushMatrix();
    glScaled(1,-1,1);
    if(!upToDate)
        calculateOutputPixels();
    textureBuffer->display();
    glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

int AbstractGraph::height()
{
    return current_display_size() / ui->widthDial->value();
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

void AbstractGraph::loadTextureCanvas(bool raggedEdge)
{
    storeDisplay( outputPixels, width(), raggedEdge );
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

bool AbstractGraph::updateDouble(double& subject, double& value)
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

int AbstractGraph::current_display_size()
{
    return min( ui->sizeDial->value(), max(0, ((int)sequence->size() - ui->startDial->value())) );
}


//***********SLOTS*******************
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
}

int AbstractGraph::width() 
{
    return ui->widthDial->value() / ui->scaleDial->value();
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

string AbstractGraph::mouseClick(point2D pt)
{
    //range check
    if( pt.x < width() && pt.x >= 0 && pt.y <= height() )
    {
        int sample_length = ui->widthDial->value();
        int index = pt.y * width() + pt.x;
        index *= ui->scaleDial->value();
        index = max(0, index);
        index = min((int)current_display_size()-sample_length-1, index);
        index = min( index + ui->startDial->value(), ((int)sequence->size()) - sample_length-1 );

        std::stringstream ss;
        ss << "Index: " << index << "  Sequence: " << sequence->substr(index, sample_length);
        //string chromosome = glWidget->chromosomeName;
        //ss<< "   <a href=\"http://genome.ucsc.edu/cgi-bin/hgTracks?hgsid=132202298&clade=mammal&org=Human&db=hg18&position="
        //<<chromosome<<":"<<index<<"-"<<index+200<<"&pix=800&Submit=submit\">View in Genome Browser</a> [external link]";
        //																											chr5			12389181	12390000
        ui->print(ss.str().c_str());
        return sequence->substr(index, min(1000, sample_length));
    }
    else{
        return string();
    }
}
