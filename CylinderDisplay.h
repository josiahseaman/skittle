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
    void createSquare();
    void quickSquare();
    void calculateOutputPixels();
    void display();
    GLuint render();
    int width();
    string SELECT_MouseClick(point2D pt);


public slots:

signals:

private:
    float max_width;
    GLuint square;
    vector<float> width_list;
    NucleotideLinker* ntLinker;
    float turnCylinder;

};

#endif
