#ifndef OLIG_DISP
#define OLIG_DISP

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "FrequencyMap.h"
#include "UiVariables.h"
#include "AbstractGraph.h"
#include "TextureCanvas.h"

using namespace std;

class OligomerDisplay : public FrequencyMap
{
    Q_OBJECT
    
public:
	OligomerDisplay(UiVariables* gui, GLWidget* gl);
	~OligomerDisplay();
	void createConnections();
	void checkVariables();
	void display();
	void load_canvas();
	GLuint render();
	void freq_map();
	int height();
	void mouseClick(point2D pt);
	
	void display_freq();
	void calculate(vector<color>& img, int vote_size);
	vector<color> calculateBoundaries(vector<color>& img, int row_size, int graphWidth);
	double correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample);
	int width();
	
	inline int ACGT_num(char n)
	{
		if(n == 'A') return 0;
		if(n == 'T') return 3;
		if(n == 'C') return 1;
		if(n == 'G') return 2;
	
		return -100000;//handles up to 8-mer correctly
	}
	
public slots:	
	void changeWordLength(int);
	void toggleVisibility();
signals:
	void wordLengthChanged(int);
	
private:
	TextureCanvas* graphBuffer;
	int wordLength;
	int similarityGraphWidth;
};

#endif
