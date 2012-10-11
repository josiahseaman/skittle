#ifndef FREQ_DISP
#define FREQ_DISP

//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "NucleotideDisplay.h"
#include "UiVariables.h"

using namespace std;

class RepeatMap : public AbstractGraph 
{
    Q_OBJECT
    
public:	
	RepeatMap(UiVariables* gui, GLWidget* gl);
    ~RepeatMap();
	QScrollArea* settingsUi();
	void display();
    void print_scores(vector<float> scores_3mer);
	void link(NucleotideDisplay* nuc_display);
	void load_canvas();
	GLuint render();
	void freq_map();
    void max_vertical_pair_filter();
    void min_3mer_filter();
    vector<float> convolution_3mer();
	int height();
	string mouseClick(point2D pt);
	
	vector<point> bestMatches();
	void display_freq();
	void calculate(vector<color>& img, int vote_size);
	double correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample);
	int width();

    vector<vector<float> > emptyCopy(vector<vector<float> > starter);
public slots:
	void changeFStart(int val);
	void changeGraphWidth(int val);
    void toggleDoubleSample(bool d);
    void toggleMin3merFilter(bool m);
	
signals:
	void fStartChanged(int);
	void graphWidthChanged(int);
	
protected:
	NucleotideDisplay* nuc;
	GLuint display_object;
	vector< vector<float> > freq;
	vector<color> pixels;
	int F_width;
	int F_start;
	int F_height;
	
	int freq_map_count;
	int calculate_count;
    bool usingDoubleSampling;
    bool usingMin3mer;
};

#endif
