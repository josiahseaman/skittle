#ifndef TRACK_DISP
#define TRACK_DISP

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "MainWindow.h"

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
	void newTrack(const vector<track_entry>* track);
	void addEntry(vector<track_entry>& activeTracks, track_entry item);
	void mouseClick(point2D pt);
	unsigned int width() const{
		return max_width;
	}
	
public slots:	
	
signals:

private:
	GLWidget* glWidget;
	unsigned int max_width;
	GLuint display_object;
	vector<track_entry> gtfTrack;
};

#endif
