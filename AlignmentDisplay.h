//AlignmentDisplay.h
#ifndef ALIGNMENT_DISP
#define ALIGNMENT_DISP

#include <QtOpenGL/QGLWidget>
#include <vector>
#include "BasicTypes.h"
#include "UiVariables.h"
#include "AbstractGraph.h"

using namespace std;

class AlignmentDisplay : public AbstractGraph
{
    Q_OBJECT
    
public:
	
	AlignmentDisplay(UiVariables*, GLWidget* gl);
	void display();
	void loadTexture();//calls simple alignment
	GLuint render();//creates display list using loadTexture
	
	void mergeMatches(vector<color>& original, vector<point>& vlr);
	void VLRcheck(vector<point> matches);
	
	void displayLegend(float canvasWidth, float canvasHeight);
	color alignment_color(int score, int frequency);
	color interpolate(color p1, color p3, double progress);
	
	long int& accessLI(int index);
	long int sequenceLI(int index);
	int countMatches(long int bits);
		
	int countMatchesShort(unsigned short int bits);
	int countMatchesChar(unsigned char bits);
	void calcMatchTable();
	void normalPack(const string* seq);
	void shiftMask(char* str, int size);
	void shiftString(unsigned char* str, int size);
	color simpleAlignment(int index);
	void setSequence(const string* seq);

public slots:
	void changeScale(int s);
	void toggleVisibility();
	
private:	
	unsigned int charPerIndex;
	int* countTable;
	int* countTableShort;
	int* countTableChar;
	unsigned char* packSeq;
	int pSeqSize;

	GLuint display_object;
	GLWidget* glWidget;
	
};

inline
long int& AlignmentDisplay::accessLI(int index)
{
	unsigned char* ptr = &(packSeq[pSeqSize - index - sizeof(long int)]);	
	return  *( (long int*)(ptr) );
}

inline
long int AlignmentDisplay::sequenceLI(int index)
{
	int sub_index = index % charPerIndex;
	index /= charPerIndex;
	
	long int start = accessLI(index);
	if(sub_index)//skip all the shifting if we don't have to do it
	{
		int inc = sub_index * 2;
		start = start << inc;
		unsigned long int end = accessLI(index + sizeof(long int) );//sign bit has a bad tendency to get duplicated
		end = end >> (32 - inc);
		start += end;
	}
	return start;
}

inline
int AlignmentDisplay::countMatches(long int bits)
{
	//TODO: can I do this by pointer casting?
	int c = countTable[ (unsigned short int)bits ];
	unsigned short int b = bits >> 16;
	c += countTable[ b ];
	
	return c;
}


#endif
