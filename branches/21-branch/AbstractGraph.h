#ifndef ABSTRACT_GRAPH
#define ABSTRACT_GRAPH

#include <QGLWidget>
#include <string>
#include "BasicTypes.h"
/**
*  This is the base class for all Grapher objects.  The constructor requires
*  a Ui reference so that it can 'connect' to controls in the Ui.
*/

class Ui_SkittleGUI;

using namespace std;

class AbstractGraph : public QGLWidget
{
    Q_OBJECT	
	
protected:			
	int Width;

public:
	Ui_SkittleGUI* ui;	
	const string* sequence;
	
	bool hidden;
	bool upToDate;
	
	int display_size;
	int max_display_size;
	int nucleotide_start;
	int scale;
	
	 int width() const{
		return Width;
	}
	
	void checkVariables();
	point get_position(int index);
	int height();
	void paint_square(point position, color c);
	bool updateInt(int& subject, int& value);
	bool useTextureOptimization();
	virtual GLuint render() = 0;

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
