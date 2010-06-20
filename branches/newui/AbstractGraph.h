#ifndef ABSTRACT_GRAPH
#define ABSTRACT_GRAPH

#include <QGLWidget>
#include <string>
#include <vector>
#include "BasicTypes.h"
/**
*  This is the base class for all Grapher objects.  The constructor requires
*  a Ui reference so that it can 'connect' to controls in the Ui.
*/

class GLWidget;
class QFrame;
class QAction;
class UiVariables;

using namespace std;

class AbstractGraph : public QGLWidget
{
    Q_OBJECT	
	
protected:			
	int Width;
	int frameCount;
	const string* sequence;
	QFrame* settingsTab;

public:
	GLWidget* glWidget;
	UiVariables* ui;	
	
	
	bool hidden;
	bool upToDate;
	QAction* toggleButton;
	
	int display_size;
	int max_display_size;
	int nucleotide_start;
	int scale;
	string actionLabel;
	string actionTooltip;
	string actionData; 
	
	~AbstractGraph();
	virtual int width();
	
	virtual void createConnections();
	virtual point get_position(int index);
	virtual int height();
	virtual void paint_square(point position, color c);
	virtual bool updateInt(int& subject, int& value);
	virtual bool updateVal(double& subject, double& value);
	virtual void display() = 0;
	virtual void displayLegend(float canvasWidth, float canvasHeight);
	virtual void checkVariables();
	virtual GLuint render() = 0;
	virtual void setButtonFont();
	virtual void setSequence(const string* seq);
	virtual string mouseClick(point2D pt);
	virtual string getFileName();
	virtual QFrame* settingsUi();
	

public slots:
	void changeScale(int s);
	void changeSize(int s);
	void changeStart(int s);
	void changeWidth(int w);
	void invalidate();
	void toggleVisibility();
	
signals:
	void displayChanged();
	void hideSettings(QFrame*);
	void showSettings(QFrame*);
	void scaleChanged(int);
	void sizeChanged(int);	
	void startChanged(int);
	void widthChanged(int);	
	void deleteButton(QAction*);
	
};

#endif
