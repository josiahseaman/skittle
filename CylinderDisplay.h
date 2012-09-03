#ifndef CYLINDER_DISP
#define CYLINDER_DISP

//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "UiVariables.h"
#include "NucleotideLinker.h"

using namespace std;

class CylinderDisplay : public AbstractGraph 
{
	Q_OBJECT

public:
	CylinderDisplay(UiVariables* gui, GLWidget* gl);
	~CylinderDisplay();
	void createConnections();
	void createSquare();
	void quickSquare();
	void display();
	GLuint render();
	int width();
	string mouseClick(point2D pt);
	
	
public slots:	
	void saySomething();
	
signals:

private:
	float max_width;
	GLuint display_object;
	GLuint square;
	vector<float> width_list;
	NucleotideLinker* ntLinker;
	float turnCylinder;
	
};

#endif
