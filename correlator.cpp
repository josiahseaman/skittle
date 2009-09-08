//Scalable Frequency Map using Pixel Compression and Color Cross Correlation
#include "correlator.h"
#include "TextureCanvas.h"
#include "NucleotideDisplay.h"
#include "TextureCanvas.h"

correlator::correlator(NucleotideDisplay* g)
{

	grapher = g;
	ui = g->ui;

	freq_up2date = false;
	colors_up2date = false;
	F_start = 0;
	F_width = 250;
	vote_size = 200;
	display_size = 0;//set when the first img is passed to color_overview_freq
	color_size = 1;
	textureBuffer = NULL;
}




void correlator::display_color_overview()
{	
	int disp_width = width / color_size;	
	disp_width = max( 1, disp_width);  // no / 0 allowed

	if(colors_up2date == false)
	{	
		freq_up2date = false;
		color_overview();
	}
	if(grapher->useTextureOptimization())
	{
		TextureCanvas canvas = TextureCanvas(color_avgs, disp_width);
		canvas.display();
	}
	else
	{
		for( unsigned int h = 0; h < color_avgs.size(); h++)
		{	
			grapher->paint_square(point(h%disp_width, h/disp_width, 0) , color_avgs[h]);		
		}
	}
}
//UNUSED
void correlator::dump_values()//displays the already constructed map
{
	if(! freq_up2date )
	{
		color_overview_freq();
	}
	int F_height = check_height();
	for( int h = 0; h < F_height; h++)
	{
		//calculate across widths 
		for(int w = 1; w <= F_width; w++)
		{
			double grey = freq[h][w];
			cout << grey << "\n";
		}
	}	

}
//UNUSED
void correlator::displayAlignment()
{
	ui->print("displayAlignment");
	//if(!freq_up2date || !textureBuffer)
		loadAlignment();
	
	textureBuffer->display();
}
//UNUSED
void correlator::loadAlignment()
{
	ui->print("loadAlignment");
	vector<color> Fmap = colorByAlignment();
	if(textureBuffer)
		delete textureBuffer;
	ui->print("Size: ", Fmap.size());
	textureBuffer = new TextureCanvas( Fmap, F_width );
}
