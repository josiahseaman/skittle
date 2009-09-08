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
	int check_height();	
	vector<color> color_compress(vector<color> sample, int squish_factor);
	vector<color> color_max(vector<color> sample);
		
	void display_color_overview();
	void color_overview();
	
	void display_freq();
	void dump_values();
	void displayAlignment();
	void loadAlignment();
	vector<color> colorByAlignment();
	void color_overview_freq();
	void color_overview_freq(vector<color>& img, int Width);
	double correlate(vector<color>& img, int beginA, int beginB, int Vote_Size);
	double correlate(vector<color>& A, vector<color>& B, int Vote_Size);

private:
	TextureCanvas* textureBuffer;
	Ui_SkittleGUI* ui;

};
#endif
