#ifndef CYLINDER_DISP
#define CYLINDER_DISP

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "SkittleUi.h"
#include "NucleotideLinker.h"

using namespace std;

class CylinderDisplay : public AbstractGraph 
{
	Q_OBJECT

public:
	CylinderDisplay(Ui_SkittleGUI* gui);
	~CylinderDisplay();
	void createConnections();
	void createSquare();
	void quickSquare();
	void display();
	GLuint render();
	float maxWidth();
	
	
	inline int actualWidth()
	{
		return scale * width();
	}
	
public slots:	
	void saySomething();
	
signals:

private:
	float max_width;
	GLuint display_object;
	GLuint square;
	vector<color> nucleotide_colors;
	vector<float> width_list;
	NucleotideLinker* ntLinker;
	float turnCylinder;
	
};

#endif
