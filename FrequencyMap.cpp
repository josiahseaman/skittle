#include "FrequencyMap.h"
#include "glwidget.h"
#include <sstream>
#include <QFrame>


FrequencyMap::FrequencyMap(UiVariables* gui, GLWidget* gl)
{	
	glWidget = gl;
	ui = gui;
	nuc = NULL;
	string* seq = new string("AATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATT");	
	sequence = seq;
	textureBuffer = NULL;
	toggleButton = NULL;
	hidden = true;

	nucleotide_start = 1;
	F_width = 250;
	F_start = 0;
	F_height = 0;
	Width = ui->widthDial->value();
	changeSize(ui->sizeDial->value());
	upToDate = false;

	freq = vector< vector<float> >();
	for(int i = 0; i < 400; i++)
	{
		freq.push_back( vector<float>(F_width, 0.0) );
	}
	freq_map_count = 0;
	calculate_count = 0;
	
	actionLabel = string("Repeat Map");
	actionTooltip = string("Graph of possible alignmments");
	actionData = actionLabel; 
	display_object = 0;
}

FrequencyMap::~FrequencyMap()
{
    glDeleteLists(display_object, 1);
}

/**IMPORTANT: When referring to the parent ui->glWidget,              //
* it is not fully constructed yet.
* 
* these connections should be placed in glWidget's constructor.    
* 
* See note in AbstractGraph::createConnections
*                                                                   ***/
void FrequencyMap::createConnections()
{
	connect( this, SIGNAL(widthChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(startChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(sizeChanged(int)), this, SIGNAL(displayChanged()));
	
	connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
	//connect( this, SIGNAL(widthChanged(int)), ui->widthDial, SLOT(setValue(int)));//width dial = Width
	
	connect( ui->startDial, SIGNAL(valueChanged(int)), this, SLOT(changeStart(int)));
	connect( this, SIGNAL(startChanged(int)), ui->startDial, SLOT(setValue(int)));
	
	connect( ui->sizeDial, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect( this, SIGNAL(sizeChanged(int)), ui->sizeDial, SLOT(setValue(int)));
	
	connect( ui->scaleDial, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
}

QScrollArea* FrequencyMap::settingsUi()
{	
    settingsTab = new QScrollArea();    
    settingsTab->setWindowTitle(QString("Repeat Map Settings"));
	QFormLayout* formLayout = new QFormLayout;
	formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
	settingsTab->setLayout(formLayout);
    
	QSpinBox* graphStartDial = new QSpinBox(settingsTab);
	graphStartDial->setMinimum(1);	
	graphStartDial->setMaximum(100000);	
	graphStartDial->setSingleStep(10);
	formLayout->addRow("Starting Offset:", graphStartDial);
	
	connect( graphStartDial, SIGNAL(valueChanged(int)), this, SLOT(changeFStart(int)));	
	connect( this, SIGNAL(fStartChanged(int)), graphStartDial, SLOT(setValue(int)));
	connect( this, SIGNAL(fStartChanged(int)), this, SIGNAL(displayChanged()));
    
    QSpinBox* graphWidthDial  = new QSpinBox(settingsTab);
    graphWidthDial->setMinimum(1);
    graphWidthDial->setMaximum(250);
    graphWidthDial->setSingleStep(10);
    graphWidthDial->setValue(F_width);
	formLayout->addRow("Graph Display Width:", graphWidthDial);
	
	connect( graphWidthDial, SIGNAL(valueChanged(int)), this, SLOT(changeGraphWidth(int)));
	connect( this, SIGNAL(graphWidthChanged(int)), graphWidthDial, SLOT(setValue(int)));
	connect( this, SIGNAL(graphWidthChanged(int)), this, SIGNAL(displayChanged()));
	
	return settingsTab;
}

void FrequencyMap::display()
{
	checkVariables();
	
	if( !upToDate )
	{
		if((scale > 1)&& nuc != NULL)
		{
			nuc->checkVariables();
			if(!nuc->upToDate)
			{
				nuc->load_nucleotide();
			}
			int displayWidth = ui->widthDial->value() / scale;
			calculate(nuc->nucleotide_colors, displayWidth);
		}
		else
		{
			freq_map();	
		}
	}
	load_canvas();
	glPushMatrix();
		glScaled(1,-1,1);
		textureBuffer->display();
	glPopMatrix();

	//Draw Red indicator according to Width
	int displayWidth = ui->widthDial->value() / ui->scaleDial->value(); 
	glPushMatrix();
		glColor4f(1,0,0, 1);//red
	    glTranslated(displayWidth - F_start -1, 202, 0);
	    glScaled(.5, 410, 1);
	    paint_square(point(-1, 0, .25), color(255,0,0));
	    paint_square(point(1, 0, .25), color(255,0,0));
	glPopMatrix();

}

void FrequencyMap::link(NucleotideDisplay* nuc_display)
{
	nuc = nuc_display;
}

void FrequencyMap::load_canvas()
{
	pixels.clear();
	for( int h = 0; h < height(); h++)
	{		
		for(int w = 1; w <= F_width; w++)
		{
			int grey = static_cast<int>(  freq[h][w] * 255 );			
			pixels.push_back( color(grey, grey, grey) );
		}
	}
	storeDisplay(pixels, F_width);

	upToDate = true;
}

GLuint FrequencyMap::render()
{
	if(! upToDate )
		freq_map();	

	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
	glPushMatrix();
	glScaled(1,-1,1);
		if(!upToDate)
			load_canvas();	
		textureBuffer->display();	
	glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void FrequencyMap::freq_map()
{
	//glWidget->print("Freq_map: ", ++freq_map_count);
	const char* genome = sequence->c_str() + nucleotide_start;
	for( int h = 0; h < height(); h++)
	{
		int offset = h * Width;
		/*int end = offset+Width-1;
		//NOTE: This statement is just an optional speed up
		if(genome[offset] == 'N' || genome[offset] == 'n' || genome[end] == 'N' || genome[end] == 'n')
		{//check the first and last of the reference string
			for(int w = 1; w <= F_width; w++)
				freq[h][w] = 0;//set whole row to zero
		}
		else
		{*/	
			for(int w = 1; w <= F_width; w++)//calculate across widths 1-F_width
			{
				int score = 0;
				for(int l = 0; l < Width; l++)
				{					
					if(genome[offset + l] == genome[offset + w + F_start*scale + l])				
						score += 1; //pixel matches the one above it			
				}
				freq[h][w] = float(score) / Width;
			}
		//}
	}	
	upToDate = true;
}

int FrequencyMap::height()
{		
	F_height = (((long int)display_size) - F_start*scale - F_width*scale ) / Width;

	F_height = max(0, min(400, F_height) );
	
	return F_height;
}

/******SLOTS*****/
void FrequencyMap::changeFStart(int val)
{
	if(updateInt(F_start, val))
		emit fStartChanged(F_start);
}

void FrequencyMap::changeGraphWidth(int val)
{
	val = min(250, val);
	if(updateInt(F_width, val))
	{
		//freq.clear();
		//freq = vector< vector<float> >();
		for(int i = 0; i < 400; i++)
		{
			freq[i].clear();
		}
		for(int i = 0; i < 400; i++)
		{
			freq[i] = vector<float>(F_width, 0.0) ;
		}
		
		emit graphWidthChanged(F_width);
	}
}	

string FrequencyMap::mouseClick(point2D pt)
{
	//range check
	if( pt.x < (int)width() && pt.x >= 0 && pt.y <= height() )
	{
		pt.x += 1;//+1 because offset 1 is the first pixel [0]
		pt.x *= scale;
		int index = pt.y * Width;
		index = index + nucleotide_start;
		int index2 = index + pt.x + F_start;
		int w = min( 100, ui->widthDial->value() );
		if( index2 + w < sequence->size() )
		{
			stringstream ss;
			ss << "Offset: "<<pt.x+ F_start<<" #" << index << " compared with #" << index2 << "  \n"
				<< sequence->substr(index, w) << "\n <----> \n" << sequence->substr(index2, w);
			ui->print(ss.str());
		}
		
		//ui->widthDial->setValue( pt.x);
	}
	return string();
}

/***Correlation***/
vector<point> FrequencyMap::bestMatches()
{
	//calculate(color_avgs, width() / ui->scaleDial->value());
		
	vector<point> best_matches;
	for(int h =0; h < height(); h++)
	{
		float best_score = 0;
		int best_freq = 0;
		//if(freq[h][1] != 0)//N's block
		{
			for(int w = 1; w <= F_width; w++)
			{
				float curr = freq[h][w];
				if( curr * .90 > best_score)  //new value must beat old by at least 10%
				{
					best_score = curr;
					best_freq = w;
				}
			}
		}
		if(best_freq == 0) best_freq = 1;//NNNNN's show up as 0 and causes crash
		best_matches.push_back( point(best_score, (float)best_freq, 0) );
	}
	return best_matches;
}

void FrequencyMap::calculate(vector<color>& img, int pixelsPerSample)//constructs the frequency map
{
	//display_size = img.size();
	checkVariables();
	//glWidget->print("Calculate(): ", ++calculate_count);
	for( int h = 0; h < height(); h++)
	{
		int offset = h * pixelsPerSample;
		for(int w = 1; w <= F_width; w++)//calculate across widths 1-F_width
		{
			freq[h][w] = .5 * (1.0 + correlate(img, offset, offset+w, pixelsPerSample));
		}
	}
	upToDate = true;
}

double FrequencyMap::correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample)//calculations for a single pixel
{//correlation will be a value between -1 and 1 representing how closley related 2 sequences are
	//calculation variables!!!  should all be double to prevent overflow
	double N = pixelsPerSample;
	int AVal;		int BVal;
	
	double ARedsum = 0,	AGreensum = 0,		ABluesum = 0;   //our tuple of color sums
	double BRedsum = 0,	BGreensum = 0,		BBluesum = 0;   //our tuple of color sums
	double ASquaredRed = 0,	ASquaredGreen = 0,	ASquaredBlue = 0;  //this is Aij^2
	double BSquaredRed = 0,	BSquaredGreen = 0,	BSquaredBlue = 0;  //this is Bij^2	
	double ABRed = 0, 	ABGreen =0,		ABBlue =0;	//this is A[]*B[]

	for (int k = 0; k < pixelsPerSample; k++)
	{//3 color shades RGB,  2 samples A and B
		//reds
		color A = img[beginA + k];
		color B = img[beginB + k];
		AVal = A.r;							BVal = B.r;
		ARedsum += AVal;					BRedsum += BVal;
		ASquaredRed += (AVal*AVal);			BSquaredRed += (BVal*BVal);
		ABRed += (AVal * BVal);
		//Greens
		AVal = A.g;							BVal = B.g;
		AGreensum += AVal;					BGreensum += BVal;
		ASquaredGreen += (AVal*AVal);		BSquaredGreen += (BVal*BVal);
		ABGreen += (AVal * BVal);
		//Blues
		AVal = A.b;							BVal = B.b;
		ABluesum += AVal;					BBluesum += BVal;
		ASquaredBlue += (AVal*AVal);		BSquaredBlue += (BVal*BVal);
		ABBlue += (AVal * BVal);				
						
	}   
	
	//calculation time
	double AbarRed = 0,	AbarGreen = 0,			AbarBlue = 0;  //A-tuple for color means
	double BbarRed = 0,	BbarGreen = 0,			BbarBlue = 0;  //B-tuple for color means
	AbarRed = ARedsum / N;	AbarGreen = AGreensum / N;	AbarBlue = ABluesum / N;
	BbarRed = BRedsum / N;	BbarGreen = BGreensum / N;	BbarBlue = BBluesum / N;
	
	double numerator_R = ABRed   - BbarRed   * ARedsum   - AbarRed * BRedsum     + AbarRed   * BbarRed   * N;
	double numerator_G = ABGreen - BbarGreen * AGreensum - AbarGreen * BGreensum + AbarGreen * BbarGreen * N;
	double numerator_B = ABBlue  - BbarBlue  * ABluesum  - AbarBlue * BBluesum   + AbarBlue  * BbarBlue  * N;	
	
	double denom_R1 = (sqrt(ASquaredRed   - ((ARedsum   * ARedsum)  /N)));
	double denom_R2 = (sqrt(BSquaredRed   - ((BRedsum   * BRedsum)  /N)));
	double denom_G1 = (sqrt(ASquaredGreen - ((AGreensum * AGreensum)/N)));
	double denom_G2 = (sqrt(BSquaredGreen - ((BGreensum * BGreensum)/N)));
	double denom_B1 = (sqrt(ASquaredBlue  - ((ABluesum  * ABluesum) /N)));
	double denom_B2 = (sqrt(BSquaredBlue  - ((BBluesum  * BBluesum) /N)));
	
	double backup = sqrt(1 - (1/N));//if we have 0 instances of a color it will be / 0  div0
	if(denom_R1 == 0) denom_R1 = backup;
	if(denom_R2 == 0) denom_R2 = backup;
	if(denom_G1 == 0) denom_G1 = backup;
	if(denom_G2 == 0) denom_G2 = backup;
	if(denom_B1 == 0) denom_B1 = backup;
	if(denom_B2 == 0) denom_B2 = backup;

	double answer_R = numerator_R / (denom_R1 * denom_R2);
	double answer_G = numerator_G / (denom_G1 * denom_G2);
	double answer_B = numerator_B / (denom_B1 * denom_B2);

	return (answer_R + answer_G + answer_B)/3;//return the average of RGB correlation
}

int FrequencyMap::width()
{
	return F_width;
}
