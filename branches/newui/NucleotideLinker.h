#ifndef NLINK
#define NLINK

//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <math.h>
#include "NucleotideLink.h"

#define PI 3.141592653589793

using namespace std;

class NucleotideLinker
{
public:
	int F_width;
	int min_width;
	vector<NucleotideLink> links;

	NucleotideLinker();
	/*
	inline NucleotideLink operator[] (int index)
	{
		return link_map[ (index/8) * 8 ];//round down index
	}*/
		
	void display();
	
	void calculate(string sequence, int min = 200);
	
	vector<float> smooth( int preferred_width, int blur_size);
	
//	void tie_up_loose_ends(vector<float>& width_list);
private:
	void draw_arc(int distance);
	
	void DrawArc(float r, float cx = 0, float cy = 0, float start_angle = PI, float arc_angle= PI, int num_segments = 0);
	
	NucleotideLink multiples_check(const string& seq, NucleotideLink match, int consensus_width);
	
	NucleotideLink localized_search(const string& seq, int start, int dist, unsigned int range);
	
	void check_for_max(const string& seq, int dist, int index, short int& max, int& best_distance);
	
	float average( const list<int>& window );
	
};



#endif
