#ifndef ABSTRACT_GRAPH
#define ABSTRACT_GRAPH

#include <QGLWidget>
#include <string>
#include "BasicTypes.h"
#include "UiVariables.h"
/**
*  This is the base class for all Grapher objects.  The constructor requires
*  a Ui reference so that it can 'connect' to controls in the Ui.
*/

class GLWidget;

using namespace std;

class AbstractGraph : public QGLWidget
{
    Q_OBJECT	
	
protected:			
	int Width;
	int frameCount;
	const string* sequence;

public:
	GLWidget* glWidget;
	UiVariables* ui;	
	
	bool hidden;
	bool upToDate;
	
	int display_size;
	int max_display_size;
	int nucleotide_start;
	int scale;
	
	virtual int width();
	
	virtual void createConnections();
	virtual point get_position(int index);
	virtual int height();
	virtual void paint_square(point position, color c);
	virtual bool updateInt(int& subject, int& value);
	virtual bool useTextureOptimization();
	virtual void display() = 0;
	virtual void checkVariables();
	virtual GLuint render() = 0;
	virtual void setSequence(const string* seq);
	virtual void mouseClick(point2D pt);
	virtual string getFileName();
	
	string actionLabel;
	string actionTooltip;
	string actionData; 

public slots:
	void changeScale(int s);
	void changeSize(int s);
	void changeStart(int s);
	void changeWidth(int w);
	void invalidate();
	void toggleVisibility();
	
signals:
	void displayChanged();
	void scaleChanged(int);
	void sizeChanged(int);	
	void startChanged(int);
	void widthChanged(int);	
	
};

#endif
