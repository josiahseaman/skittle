//HighlightDisplay.h
#ifndef HIGH_DISP
#define HIGH_DISP

#include <QGLWidget>
#include <QString>
#include "NucleotideDisplay.h"
#include <string>
#include <vector>

using namespace std;

class HighlightDisplay : public NucleotideDisplay
{
	Q_OBJECT

public:	
	HighlightDisplay(UiVariables* gui, GLWidget* gl);
	~HighlightDisplay();
	void display();
	GLuint render();
	void assignColors();
	void calculate();
	
public slots:
	void setHighlightSequence(const QString&);
	void setPercentSimilarity(int);
	
private:
	vector<unsigned short int> similarity;
	string target;
	double percentage_match;
/*	
public slots:	
	void changeWidth(int w);
	void mouseClick(point2D pt);
	
signals:

private:
	
*/
};

#endif
