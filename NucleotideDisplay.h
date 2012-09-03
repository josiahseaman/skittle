#ifndef NUC_DISP
#define NUC_DISP

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

using namespace std;

class NucleotideDisplay : public AbstractGraph 
{
	Q_OBJECT

public:
	vector<color> nucleotide_colors;
	bool invert;
	bool C;
	bool G;
	bool A;
	bool T;	
	
	NucleotideDisplay(UiVariables* gui, GLWidget* gl);
	~NucleotideDisplay();
	void createConnections();
	virtual void display();
	virtual GLuint render();
	void load_nucleotide();
	void loadTextureCanvas();
	void color_compress();
	int widthInBp();//Nucleotide Display changes Width internally to w/scale
	
public slots:	
//	void changeWidth(int w);
	string mouseClick(point2D pt);
	
signals:

protected:
	GLuint display_object;
};

#endif
