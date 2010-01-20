#ifndef FREQ_DISP
#define FREQ_DISP

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "AbstractGraph.h"
#include "SkittleUi.h"
#include "TextureCanvas.h"

using namespace std;

class FrequencyMap : public AbstractGraph 
{
    Q_OBJECT
    
public:
	
	bool texture_optimization;
	
	FrequencyMap(Ui_SkittleGUI* gui);
	~FrequencyMap();
	void createConnections();
	void checkVariables();
	void display();
	void load_canvas();
	GLuint render();
	void freq_map();
	int check_height();
	void mouseClick(point2D pt);
	
	vector<point> bestMatches();
	void display_freq();
	void calculate(vector<color>& img, int vote_size);
	double correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample);
	
	inline int width() const
	{
		return F_width;
	}
	
public slots:	
	
signals:

private:
	GLuint display_object;
	vector< vector<float> > freq;
	TextureCanvas* textureBuffer;
	vector<color> pixels;
	int F_width;
	int F_start;
	int F_height;
	
	int freq_map_count;
	int calculate_count;
};

#endif
