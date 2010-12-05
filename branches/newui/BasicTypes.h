#ifndef BASIC_TYPES
#define BASIC_TYPES

#include <math.h>
#include <QtGui>
#include <string>

using namespace std;

class ErrorBox{
public:
	ErrorBox(const char* msg)
	{
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
	}
};

class color{
public:
	int r;
	int g;
	int b;
	color()
	{
		r = 0; g = 0; b = 0;
	}
	color(int grey)
	{
		r = grey; g = grey; b = grey;
	}
	color(double zeroToOne)
	{
		int grey = (int)(zeroToOne*255);
		r = grey; g = grey; b = grey;
	}
	color(int red, int green, int blue){
		r=red;
		g=green;
		b=blue;
	}
	bool operator == (color other)
	{
		return ( (r == other.r) && (g == other.g) && (b == other.b) );
	}
	color operator + (color other)
	{
		return color( min(255, r + other.r), min(255, g + other.g), min(255, b + other.b) );
	}
	color operator - (color other)
	{
		return color( max(0,(r - other.r)), max(0,(g - other.g)), max(0,(b - other.b)) );
	}
	color operator / (int div)
	{
		return color( (int)(float(r)/div+.5), (int)(float(g)/div+.5), (int)(float(b)/div+.5) );
	}
	color operator * (float mul)
	{
		return color( min(255, (int)(float(r)*mul+.5)), min(255, (int)(float(g)*mul+.5)), min(255, (int)(float(b)*mul+.5)) );
	}
	int lightness()
	{
		int a = max(r, g);
		return max( a, b);
	}
};

class point{
public:
	double x;
	double y;
	double z;
	point(){x=0; y=0; z=0;}
	point(double X, double Y, double Z){
		x=X;
		y=Y;
		z=Z;
	}
	point operator + (point other)
	{
		return point( (x + other.x), (y + other.y), (z + other.z) );
	}
	point operator / (int div)
	{
		return point( x/div, y/div, z/div );
	}
	point interpolate(point p3, double progress)//progress goes from 0.0 p1  to 1.0 p2
	{
		double inverse = 1.0 - progress;
		float x2 = x * inverse + p3.x * progress;
		float y2 = y * inverse + p3.y * progress;
		float z2 = z * inverse + p3.z * progress;
		return point(x2, y2, z2);
	}
};

class point2D{
public:
	int x;
	int y;
	point2D(){}
	point2D(int X, int Y)
	{
		x = X;
		y = Y;
	}
};

class track_entry{
public:
	int start;
	int stop;
	color col;
	string line;
	int index;
	track_entry(){
		start=0; 
		stop = 0;
		col = color(0,0,0);
	}
	track_entry(int Start, int Stop, color C)
	{
		start = Start;
		stop = Stop;
		col = C;
	}
	track_entry(int Start, int Stop, color C, string Line)
	{
		start = Start;
		stop = Stop;
		col = C;
		line = Line;
	}
	bool operator < (track_entry& b)
	{
		if(start < b.start)
			return true;
		else if( start == b.start)
			return stop < b.stop;
		else
			return false;
	}
	bool isBlank()
	{
		return ( (start==0 && stop==0 && col.r == 0 && col.g == 0 && col.b == 0) );
	}
	string toString() const
	{
		return line;
	}
};
#endif
