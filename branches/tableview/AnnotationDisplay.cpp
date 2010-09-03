#include "AnnotationDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <algorithm>

AnnotationDisplay::AnnotationDisplay(UiVariables* gui, GLWidget* gl, string gtfFileName)
{	
	glWidget = gl;
	ui = gui;
	hidden = true;

	nucleotide_start = 1;
	upToDate = false;
	max_width = 0;
	
	actionLabel = string("Annotation Display");
	actionTooltip = string("Genome annotation locations");
	actionData = actionLabel; 
	
	fileName = gtfFileName;
	display_object = 0;
}

AnnotationDisplay::~AnnotationDisplay()
{
    glDeleteLists(display_object, 1);
}

bool trackCompare(const track_entry& a, const track_entry& b)
{
	return a.start < b.start;
}

void AnnotationDisplay::newTrack(vector<track_entry> track)
{
	gtfTrack = vector<track_entry>(track);
	sort(gtfTrack.begin(), gtfTrack.end(), trackCompare);
	hidden = false;
	emit displayChanged();
}

void AnnotationDisplay::display()
{
	if( !upToDate )
	{
    	//glDeleteLists(display_object, 1);
		display_object = render();
	}	
	glCallList(display_object);
}

GLuint AnnotationDisplay::render()
{
	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
	glPushMatrix();
	
	if( !gtfTrack.empty() ) 
		displayTrack( gtfTrack );
	
	glPopMatrix();
    glEndList();
//    upToDate = true;
    return list;
}

void AnnotationDisplay::displayTrack(const vector<track_entry>& track)
{
	max_width = 1;
	int pix_start = ui->startDial->value();
	int width = ui->widthDial->value();
	int pix_stop = pix_start + width;
	int display_size = ui->sizeDial->value();
	int next_spot = 0;
	
	if(track.empty()) 
		return;
	vector<track_entry> activeEntries = vector<track_entry>();
	track_entry blank = track_entry(0,0, color(0,0,0));
	activeEntries.push_back(blank);
	for(int row = 0; row < display_size / width; row++)
	{
		//check to see if any of the old tracks are done
		for(int k = 0; k < activeEntries.size(); ++k)
		{
			if( !activeEntries[k].isBlank() )
			{
				if( activeEntries[k].stop < pix_start )
				{
					if( activeEntries[k].col == color(200,200,200) )
						activeEntries[k] = blank;//remove the entry
					else
						activeEntries[k].col = color(200,200,200);
				}
			}
		}
		//check to match start for a new track
		while(next_spot < track.size() && track[next_spot].stop < pix_start )//assumes tracks are in order
			next_spot++;
		while(next_spot < track.size() 
			&& ((track[next_spot].start >= pix_start && track[next_spot].start <= pix_stop)//start in range
			    || (track[next_spot].stop >= pix_start && track[next_spot].stop <= pix_stop)//end in range
				|| (track[next_spot].start < pix_start && track[next_spot].stop > pix_stop)) )//in the middle
		{
			stackEntry(activeEntries, track[next_spot++]);		//place new tracks in proper position
		}
		
		//display each track
		glPushMatrix();
			glScaled(1,-1,1);
		for(int x = 0; x < activeEntries.size(); ++x)
		{
			color c;
			if(activeEntries[x].isBlank())
				c = color(200,200,200); 
			else	
				c = activeEntries[x].col;
				
			glPushName( activeEntries[x].index );
			paint_square( point(x*2,row,0), c );
			paint_square( point(x*2+1,row,0), c );
			glPopName();
			if(x+1 > max_width)
				max_width = x+1;
		}
		glPopMatrix();
		pix_start += width;
		pix_stop += width;
	}
}

void AnnotationDisplay::stackEntry(vector<track_entry>& activeEntries, track_entry item)
{
		for(int k = 0; k < activeEntries.size(); ++k)
		{
			if( activeEntries[k].isBlank() )
			{
				activeEntries[k] = item;
				return;
			}
		}//we reached the end without finding an open spot
		activeEntries.push_back(item);
}

string AnnotationDisplay::mouseClick(point2D pt)
{
	//range check
	if( pt.x <= width() && pt.x >= 0 )
	{
		ui->print("-------------");
		int start = ui->startDial->value() + pt.y * ui->widthDial->value() + pt.x;
		int stop = start + ui->widthDial->value();
		if(!gtfTrack.empty())
		{
			for(int i = 0; i < gtfTrack.size(); ++i)
			{
				if(((gtfTrack[i].start >= start && gtfTrack[i].start <= stop)//start in range
				    || (gtfTrack[i].stop >= start && gtfTrack[i].stop <= stop)//end in range
					|| (gtfTrack[i].start < start && gtfTrack[i].stop > stop)) )//in the middle
				{
					ui->print(gtfTrack[i].toString().c_str());
				}
			}
		}
	}
	return string();
}

int AnnotationDisplay::width()
{
	return (int)max_width*2;
}


string AnnotationDisplay::getFileName()
{
	return fileName;
}

void AnnotationDisplay::setFileName(string gtfFileName)
{
	fileName = gtfFileName;
}

/******SLOTS*****/
void AnnotationDisplay::addEntry(track_entry entry)
{
	gtfTrack.push_back(entry);
	sort(gtfTrack.begin(), gtfTrack.end(), trackCompare);
	emit displayChanged();
}
/**/
