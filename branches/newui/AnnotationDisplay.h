#ifndef TRACK_DISP
#define TRACK_DISP

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "UiVariables.h"

using namespace std;

class AnnotationDisplay : public AbstractGraph 
{
    Q_OBJECT
    
public:
	
	AnnotationDisplay(UiVariables* gui, GLWidget* gl);
	~AnnotationDisplay();
	void display();
	GLuint render();
	void displayTrack(const vector<track_entry>& track);
	void newTrack(vector<track_entry> track);
	void mouseClick(point2D pt);
	int width();
		
public slots:	
	
signals:

private:
	GLWidget* glWidget;
	unsigned int max_width;
	GLuint display_object;
	vector<track_entry> gtfTrack;
	
	void stackEntry(vector<track_entry>& activeEntries, track_entry item);
};

#endif
