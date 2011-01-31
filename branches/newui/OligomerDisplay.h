#ifndef OLIG_DISP
#define OLIG_DISP

//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <QGLWidget>
#include "BasicTypes.h"
#include "FrequencyMap.h"
#include "UiVariables.h"
#include "AbstractGraph.h"

class QSpinBox;

using namespace std;

class OligomerDisplay : public AbstractGraph//: public FrequencyMap// 
{
    Q_OBJECT
    
public:
	OligomerDisplay(UiVariables* gui, GLWidget* gl);
	~OligomerDisplay();
	void createConnections();
	QScrollArea* settingsUi();
	void checkVariables();
	void display();
	//vector<color> calculateAverageSignature(int, int);
	//void isochores();
	void calculateHeatMap();
	vector<double> fillHalfMatrix(vector<double>& data);
	vector<double> rotateSquareMatrix(vector<double>& data);
	void selfCorrelationMap();
	void superCorrelationMap();
	vector<double> copyVectorRange(vector<double>& stuff, int index, int length);
	vector<color> colorNormalized(vector<double> heatData);
	color redBlueSpectrum(double i);
	void load_canvas();
	GLuint render();
	void freq_map();
	int oligNum(string a);
	int height();
	string mouseClick(point2D pt);
	
	void display_freq();
	void calculate(vector<color>& img, int vote_size);
	double correlate(vector<double>& apples, vector<double>& oranges);
	void assignRanks(vector<point>& temp);
	double spearmanCorrelation(vector<double>& apples, vector<double>& oranges);
	int width();
	
	inline int ACGT_num(char n)
	{
		if(n == 'A') return 0;
		if(n == 'T') return 3;
		if(n == 'C') return 1;
		if(n == 'G') return 2;
		return -100000;//handles up to 8-mer correctly
	}
	
	inline int comp_num(char n)
	{
		if(n == 'T') return 0;
		if(n == 'G') return 1;
		if(n == 'C') return 2;
		if(n == 'A') return 3;
		return -100000;//handles up to 8-mer correctly
	}
	
public slots:	
	void changeMinDelta(double mD);	
	void changeWordLength(int);

signals:
	void wordLengthChanged(int);
	
private:
	TextureCanvas* graphBuffer;
	TextureCanvas* avgBuffer;
	TextureCanvas* heatMapBuffer;
	TextureCanvas* correlationBuffer;
	TextureCanvas* superBuffer;
	vector< vector<double> > freq;
	vector<color> pixels;
	vector<int> boundaryIndices;
	vector<double> scores;
	vector<double> correlationScores;
	int wordLength;
	int widthMultiplier;
	int similarityGraphWidth;
	double minDeltaBoundary;
	QSpinBox* oligDial;
	double min_score;
	double max_score;
	double range;
	int F_width;
	int F_height;
	
};

#endif
