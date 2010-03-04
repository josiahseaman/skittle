#include "AbstractGraph.h"
#include "UiVariables.h"
#include <math.h>


void AbstractGraph::checkVariables()
{
	changeScale(ui->scaleDial->value());
	changeWidth(ui->widthDial->value());
	changeStart(ui->startDial->value());
	changeSize(ui->sizeDial->value());
}

point AbstractGraph::get_position(int index)
{
	int x = index % Width;
	int y = index / Width;
	return point(x, y, 0);
}

int AbstractGraph::height()
{
	return display_size / Width;
}

void AbstractGraph::paint_square(point position, color c)
{
	int index = 0;
	index = static_cast<int>((position.y * Width) + position.x);
	if(index < 0) index = 0;
	
	glPushMatrix();
    	glColor3d(c.r /255.0, c.g /255.0, c.b /255.0); 
        glTranslated(position.x, position.y, position.z);
        glBegin(GL_QUADS);
	        glVertex3d(.0, .0, 0);
	        glVertex3d(-1.0, .0, 0);
	        glVertex3d(-1, -1, 0);
	        glVertex3d(.0, -1, 0);
	    glEnd();
	glPopMatrix();
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
bool AbstractGraph::useTextureOptimization()
{
	/*if(ui)
		return ui->textureCheckBox->isChecked();
	else*/
		return true;
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
		emit widthChanged(w);
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

void AbstractGraph::toggleVisibility()
{
	hidden = !hidden;
	emit displayChanged();
}

void AbstractGraph::setSequence(const string* seq)
{
	sequence = seq;	
}

void AbstractGraph::mouseClick(point2D pt){	}

int AbstractGraph::width() 
{
	return Width / scale;
}
