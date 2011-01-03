//TextRender.cpp
#include "TextRender.h"

TextRender::TextRender()
{
}

/*
void TextRender::range_labels(int start, int stop, point startX, point stopX, int granularity)
{
//GLUT_BITMAP_8_BY_13	GLUT_BITMAP_9_BY_15	GLUT_BITMAP_TIMES_ROMAN_10	GLUT_BITMAP_TIMES_ROMAN_24	GLUT_BITMAP_HELVETICA_10	GLUT_BITMAP_HELVETICA_12	GLUT_BITMAP_HELVETICA_18

	int font=(int)GLUT_BITMAP_HELVETICA_18;
	if(granularity < 1) 
		granularity = 1;
	double step =   536870912;//a very large power of 2
	while(step/2 > granularity)
		step = step / 2;
	//double Xstep = ((double)(stopX-startX)) / (stop-start) * step;
	//double x = startX;
	
	for(double i = start; i < stop; i += step)
	{		
		stringstream oss;//generate label
		oss << (int)i;	
		point p1 = startX.interpolate(stopX, (i-start)/(stop-start));
		renderBitmapCharacher(p1.x, p1.y, p1.z, (void *)font, oss.str().c_str());
		//pointer triangle
		glPushMatrix();
			glTranslated(p1.x, p1.y -1, p1.z);
			glBegin(GL_TRIANGLES);
		        glVertex3d(.5, 0, 0);
		        glVertex3d(-.5, 0, 0);
		        glVertex3d(0, -1, 0);
		    glEnd();
		glPopMatrix();
	}	
}
*/
void TextRender::renderBitmapCharacher(float x, float y, float z, void *font, const char *str)
{
	const char *c;
	//glColor3f(0,0,0);
	glRasterPos3f(x, y, z);
	for (c=str; *c != '\0'; c++) 
	{
		glutBitmapCharacter(font, *c);
	}
}



