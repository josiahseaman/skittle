#ifndef TRACK_DISP
#define TRACK_DISP

//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
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
	
	AnnotationDisplay(UiVariables* gui, GLWidget* gl, string gtfFileName);
	~AnnotationDisplay();
	void display();
	GLuint render();
    void displayLayout(vector<vector<track_entry> > layout);
    vector<vector<track_entry> > calculateTrackLayout(const vector<track_entry>& track);
	void newTrack(vector<track_entry> track);
	string mouseClick(point2D pt);
	int width();
	void setFileName(string gtfFileName);
	string getFileName();
    int current_display_size();
    int getNextAnnotationPosition();
		
public slots:
	void addEntry(track_entry entry);
	
signals:

private:
	GLWidget* glWidget;
    int max_width;
	vector<track_entry> gtfTrack;
	
	string fileName;
	void stackEntry(vector<track_entry>& activeEntries, track_entry item);
};

#endif
