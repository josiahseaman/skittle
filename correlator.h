//correlator.h
#ifndef CORRELATE
#define CORRELATE

#include <vector>
#include <algorithm>
#include "BasicTypes.h"
using namespace std;

class NucleotideDisplay;
class TextureCanvas;
class Ui_SkittleGUI;

class correlator
{
public:
	
	NucleotideDisplay* grapher;
	bool freq_up2date;
	bool colors_up2date;
	vector< vector<double> > freq;//double> freq;//
	vector<color> color_avgs;
	int F_start;
	int F_width;
	int width;
	int vote_size;
	int display_size;
	int color_size;
	

	correlator(NucleotideDisplay* g);

	void display_color_overview();

	void dump_values();

	void displayAlignment();

	void loadAlignment();

private:
	TextureCanvas* textureBuffer;
	Ui_SkittleGUI* ui;

};
#endif


