//HighlightDisplay.h
#ifndef HIGH_DISP
#define HIGH_DISP

#include <QGLWidget>
#include <QString>
#include "NucleotideDisplay.h"
#include <string>
#include <vector>

using namespace std;
class QScrollArea;
class QLineEdit;
class QGridLayout;
class SequenceEntry;

class HighlightDisplay : public NucleotideDisplay
{
	Q_OBJECT

public:	
	HighlightDisplay(UiVariables* gui, GLWidget* gl);
	~HighlightDisplay();
	QScrollArea* settingsUi();
	void display();
	GLuint render();
	vector<int> identifyMatches(string find);
	vector<unsigned short int> calculate(string find);
	void combine(vector< vector<int> >& results);
	
public slots:
	void setHighlightSequence(const QString&);
	void addNewSequence();
	void addNewSequence(string);
	void openSequence();
	void removeEntry(SequenceEntry*);
	void clearAllEntries();
	void setPercentSimilarity(int);
	
signals:
	void highlightChanged(const QString&);
	
private:
	int rowCount;
	vector<SequenceEntry*> seqLines;
	double percentage_match;
	QCheckBox* reverseCheck;
	QLineEdit* activeSeqEdit;
	QGridLayout* formLayout;
	QFrame* settingsBox;
	QPushButton* addButton;
	
	
/*	
public slots:	
	void changeWidth(int w);
	string mouseClick(point2D pt);
	
signals:

private:
	
*/
};

#endif
