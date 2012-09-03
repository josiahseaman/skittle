//TextRender.h
#ifndef TEXTRENDER_SKITTLE
#define TEXTRENDER_SKITTLE

#include <GL/glut.h>
#include <sstream>
//#include <string>
#include "BasicTypes.h"

class TextRender{
public:
	
	TextRender();
	
	void range_labels(int start, int stop, point startX, point stopX, int granularity);

	void renderBitmapCharacher(float x, float y, float z, void *font, const char *str);
};

#endif

