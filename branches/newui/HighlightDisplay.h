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
class QLineEdit;
class QFormLayout;

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
	void combine(vector< vector<int> >& results);
	string reverseComplement(string original);
	
public slots:
	void setHighlightSequence(const QString&);
	void setPercentSimilarity(int);
	void addNewSequence();
	
signals:
	void highlightChanged(const QString&);
	
private:
	vector<string> targets;
	double percentage_match;
	QCheckBox* reverseCheck;
	QLineEdit* activeSeqEdit;
	QFormLayout* formLayout;
/*	
public slots:	
	void changeWidth(int w);
	string mouseClick(point2D pt);
	
signals:

private:
	
*/
};

#endif
