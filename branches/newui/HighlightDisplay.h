//HighlightDisplay.h
#ifndef HIGH_DISP
#define HIGH_DISP

#include <QGLWidget>
#include <QString>
#include "NucleotideDisplay.h"
#include <string>
#include <vector>

using namespace std;
class QFrame;

class HighlightDisplay : public NucleotideDisplay
{
	Q_OBJECT

public:	
	HighlightDisplay(UiVariables* gui, GLWidget* gl);
	~HighlightDisplay();
	QFrame* settingsUi();
	void display();
	GLuint render();
	vector<int> identifyMatches(string find);
	vector<unsigned short int> calculate(string find);
	void combine(vector<int>& forward, vector<int>& reverse);
	string reverseComplement(string original);
	
public slots:
	void setHighlightSequence(const QString&);
	void setPercentSimilarity(int);
	
private:
	string target;
	string reverseComplementTarget;
	double percentage_match;
	QCheckBox* reverseCheck;
/*	
public slots:	
	void changeWidth(int w);
	void mouseClick(point2D pt);
	
signals:

private:
	
*/
};

#endif
