#include "AnnotationDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <algorithm>

AnnotationDisplay::AnnotationDisplay(UiVariables* gui, GLWidget* gl)
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
}

AnnotationDisplay::~AnnotationDisplay()
{
    glDeleteLists(display_object, 1);
}

bool trackCompare(const track_entry& a, const track_entry& b)
{
	return a.start < b.start;
}

void AnnotationDisplay::newTrack(const vector<track_entry>* track)
{
	gtfTrack.clear();
	gtfTrack.resize(track->size());
	for(int i = 0; i < track->size(); ++i)
	{
		gtfTrack[i] = (*track)[i];
	}
	sort(gtfTrack.begin(), gtfTrack.end(), trackCompare);
	hidden = false;
}

void AnnotationDisplay::display()
{
	if( !upToDate )
	{
    	glDeleteLists(display_object, 1);
		display_object = render();
	}	
	glCallList(display_object);
}

GLuint AnnotationDisplay::render()
{
	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE_AND_EXECUTE);
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
	
	//glWidget->print("--------------------");
	if(track.empty()) 
		return;
	vector<track_entry> activeTracks = vector<track_entry>();
	track_entry blank = track_entry(0,0, color(0,0,0));
	activeTracks.push_back(blank);
	for(int row = 0; row < display_size / width; row++)
	{
		//check to see if any of the old tracks are done
		for(int k = 0; k < activeTracks.size(); ++k)
		{
			if( !activeTracks[k].isBlank() )
			{
				if( activeTracks[k].stop < pix_start )
				{
					if( activeTracks[k].col == color(200,200,200) )
						activeTracks[k] = blank;//remove the entry
					else
						activeTracks[k].col = color(200,200,200);
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
			addEntry(activeTracks, track[next_spot++]);		//place new tracks in proper position
		}
		
		//display each track
		glPushMatrix();
			glScaled(1,-1,1);
		for(int x = 0; x < activeTracks.size(); ++x)
		{
			color c;
			if(activeTracks[x].isBlank())
				c = color(200,200,200); 
			else	
				c = activeTracks[x].col;
				
			glPushName( activeTracks[x].index );
			paint_square( point(x,row,0), c );
			glPopName();
			if(x+1 > max_width)
				max_width = x+1;
		}
		glPopMatrix();
		pix_start += width;
		pix_stop += width;
	}
}

void AnnotationDisplay::addEntry(vector<track_entry>& activeTracks, track_entry item)
{
		for(int k = 0; k < activeTracks.size(); ++k)
		{
			if( activeTracks[k].isBlank() )
			{
				activeTracks[k] = item;
				return;
			}
		}//we reached the end without finding an open spot
		activeTracks.push_back(item);
}

void AnnotationDisplay::mouseClick(point2D pt)
{
	//range check
	//if( pt.x <= width() )
	//{
		//ui->print("-------------");
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
					glWidget->print(gtfTrack[i].toString());
				}
			}
		}
}



/******SLOTS*****/

/**/
