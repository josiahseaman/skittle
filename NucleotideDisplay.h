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
    bool invert;
	
	NucleotideDisplay(UiVariables* gui, GLWidget* gl);
    ~NucleotideDisplay();
    virtual void calculateOutputPixels();
    virtual void sequenceToColors(const char* genome);
    virtual void color_compress();
	
public slots:	
//	void changeWidth(int w);
	
signals:

};

#endif
