#include "OligomerDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <math.h>
#include <QDoubleSpinBox>
#include <algorithm>

OligomerDisplay::OligomerDisplay(UiVariables* gui, GLWidget* gl)
{	
	glWidget = gl;
	ui = gui;
	string* seq = new string("AATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATT");	
	sequence = seq;
	toggleButton = NULL;
	graphBuffer = new TextureCanvas( );
	avgBuffer = new TextureCanvas();
	heatMapBuffer = new TextureCanvas();
	correlationBuffer = new TextureCanvas();
	superBuffer = new TextureCanvas();
	hidden = true;
	
	nucleotide_start = 1;
	F_width = 250;
	F_height = 0;
	changeWidth(ui->widthDial->value());
	changeSize(ui->sizeDial->value());
	upToDate = false;
	
	similarityGraphWidth = 50;
	frameCount = 0;
	minDeltaBoundary = 2.0;
	actionLabel = string("Oligomer Display");
	actionTooltip = string("Short string usage (codons = length 3)");
	actionData = actionLabel; 
	wordLength = 2;
	changeWordLength(3);
	
	connect( this, SIGNAL(wordLengthChanged(int)), this, SIGNAL(displayChanged()));//either this line or changeWordLength::invalidate()
}

OligomerDisplay::~OligomerDisplay()
{
}

/**IMPORTANT: When referring to the parent ui->glWidget, it is not fully constructed yet.
*** these connections should be placed in glWidget's constructor.    ***/
void OligomerDisplay::createConnections()
{		
			//TODO: Currently, createConnections is not called by anything
	connect( this, SIGNAL(widthChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(startChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(sizeChanged(int)), this, SIGNAL(displayChanged()));
	//connect( this, SIGNAL(wordLengthChanged(int)), this, SIGNAL(displayChanged()));
	
	connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
	//connect( this, SIGNAL(widthChanged(int)), ui->widthDial, SLOT(setValue(int)));//width dial = Width
	
	connect( ui->startDial, SIGNAL(valueChanged(int)), this, SLOT(changeStart(int)));
	connect( this, SIGNAL(startChanged(int)), ui->startDial, SLOT(setValue(int)));
	
	connect( ui->sizeDial, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect( this, SIGNAL(sizeChanged(int)), ui->sizeDial, SLOT(setValue(int)));
}


QScrollArea* OligomerDisplay::settingsUi()
{	
    settingsTab = new QScrollArea();    
    settingsTab->setWindowTitle(QString("Oligomer Settings"));
	QFormLayout* formLayout = new QFormLayout;
	formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
	settingsTab->setLayout(formLayout);
    
    oligDial = new QSpinBox(settingsTab);
    oligDial->setMinimum(1);
    oligDial->setMaximum(5);
    oligDial->setSingleStep(1);
    oligDial->setValue(3);	
	formLayout->addRow("Oligomer Length Displayed:", oligDial);
	
	connect( oligDial, SIGNAL(valueChanged(int)), this, SLOT(changeWordLength(int)));
	connect( this, SIGNAL(wordLengthChanged(int)), oligDial, SLOT(setValue(int)));
	connect( this, SIGNAL(wordLengthChanged(int)), this, SIGNAL(displayChanged()));
	
	QDoubleSpinBox* threshholdDial = new QDoubleSpinBox(settingsTab);
    threshholdDial->setMinimum(0.1);
    threshholdDial->setMaximum(10.0);
    threshholdDial->setSingleStep(.1);
    threshholdDial->setDecimals(2);
    threshholdDial->setSuffix(" stdevs");
    threshholdDial->setValue(minDeltaBoundary);	
    formLayout->addRow("Isochore Boundary Threshhold", threshholdDial);
	
	connect( threshholdDial, SIGNAL(valueChanged(double)), this, SLOT(changeMinDelta(double)));	
	
	return settingsTab;
}

void OligomerDisplay::checkVariables()
{	
	changeScale(ui->scaleDial->value());	
	changeWidth(ui->widthDial->value());//width and scale
	changeStart(ui->startDial->value());
	changeSize(ui->sizeDial->value());
	changeWordLength(oligDial->value());
}

void OligomerDisplay::changeMinDelta(double mD)
{
	if(updateVal(minDeltaBoundary, mD ))
	{
		emit displayChanged();
	}		
}

void OligomerDisplay::changeWordLength(int w)
{
	if(updateInt(wordLength, w ))
	{
		F_width = (int)pow(4, wordLength);
		freq.clear();
		freq = vector< vector<float> >();
		for(int i = 0; i < 400; i++)
		{
			freq.push_back( vector<float>(F_width, 0.0) );
		}
		//invalidate();
		emit wordLengthChanged(w);
	}
}

void OligomerDisplay::display()
{
	checkVariables();
	if(! upToDate )
	{
		freq_map();	
		load_canvas();
		calculateHeatMap();
		selfCorrelationMap();
		//superCorrelationMap();
	}
	width();
	glPushMatrix();
		glScaled(1,-1,1);
		textureBuffer->display();
		glTranslated(F_width*widthMultiplier + 2, 0, 0);
		heatMapBuffer->display();
		glTranslated(F_height + 2, 0, 0);
		correlationBuffer->display();
		//glTranslated(F_height + 2, 0, 0);
		//superBuffer->display();
	glPopMatrix();
}

void OligomerDisplay::calculateHeatMap()
{	
	scores.clear();
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = y+1; x < F_height; ++x)
		{
			int row = F_width*widthMultiplier;
			double score = correlate(pixels, y*row, x*row, row);
			scores.push_back(score);
		}	
	}
	
	vector<color> heatMap = colorNormalized(scores);
	delete heatMapBuffer;
	heatMapBuffer = new TextureCanvas( heatMap, F_height );
}
//start: 20541380  width: 479304

vector<double> OligomerDisplay::fillHalfMatrix(vector<double>& data)
{
	vector<double> filledScores;
	int k = 0;
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = 0; x < y; ++x)//filler
		{
			filledScores.push_back(filledScores[x*F_height + y]);
		}
		filledScores.push_back(1.0);		
		for(int x = y+1; x < F_height; ++x)
		{
			filledScores.push_back(data[k++]);
		}
	}
	return filledScores;
}

vector<double> OligomerDisplay::rotateSquareMatrix(vector<double>& data)
{
	vector<double> rotated;
	for(int x = 0; x < F_height; ++x)
	{	
		for(int y = 0; y < F_height; ++y)
		{	
			rotated.push_back(data[ y*F_height + x ]);
		}
	}
	return rotated;
}	

void OligomerDisplay::selfCorrelationMap()
{
	vector<double> filledScores = fillHalfMatrix(scores);
	vector<double> rotated = rotateSquareMatrix(filledScores);

	correlationScores.clear();
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = y+1; x < F_height; ++x)
		{
			int row = F_height;		
			//inputs
			vector<double> input1 = colorVectorRange(filledScores,  y*row, row); //row
			vector<double> input2 = colorVectorRange(rotated, x*row, row); //column
			input1.insert(input1.end(), input2.begin(), input2.end());
			double score = correlate(input1, 0, row, row);
			correlationScores.push_back(score);
		}	
	}
	
	vector<color> heatMap = colorNormalized(correlationScores);
	delete correlationBuffer;
	correlationBuffer = new TextureCanvas( heatMap, F_height );	
}

void OligomerDisplay::superCorrelationMap()
{
	vector<double> fill = fillHalfMatrix(correlationScores);
	vector<double> rotated = rotateSquareMatrix(fill);
	
	vector<double> superScores;
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = y+1; x < F_height; ++x)
		{
			int row = F_height;		
			//inputs
			vector<double> input1 = colorVectorRange(fill,  y*row, row); //row
			vector<double> input2 = colorVectorRange(rotated, x*row, row); //column
			input1.insert(input1.end(), input2.begin(), input2.end());
			double score = correlate(input1, 0, row, row);
			superScores.push_back(score);
		}	
	}
	
	vector<color> heatMap = colorNormalized(superScores);
	delete superBuffer;
	superBuffer = new TextureCanvas( heatMap, F_height );	
}


vector<double> OligomerDisplay::colorVectorRange(vector<double>& stuff, int index, int length)
{
	vector<double> input1;
	for(int i = index; i < length + index; ++i)
	{
		input1.push_back( stuff[i]  );
	}
	return input1;
}

vector<color> OligomerDisplay::colorNormalized(vector<double> heatData)
{
	vector<color> heatMap;
	if(heatData.empty()) return heatMap;
	
	double averageCorrelation = 0;
	vector<double> sortedScores = heatData;
	sort(sortedScores.begin(), sortedScores.end());
	double medianCorrelation = sortedScores[ sortedScores.size()/2 ];
	averageCorrelation = medianCorrelation;/*/
	averageCorrelation = averageCorrelation / (double)heatData.size();/**/
	
	double maxCorrelation = sortedScores[sortedScores.size()-1];//*(max_element(heatData.begin(), heatData.end()));
	double minCorrelation = sortedScores[0];//*(min_element(heatData.begin(), heatData.end()));
	//double midpoint = (maxCorrelation + minCorrelation)/2.0;
	
	int k = 0;
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = 0; x < y; ++x)//filler
		{
			heatMap.push_back(heatMap[x*F_height + y]);
		}
		heatMap.push_back(color(255,00,00));		
		for(int x = y+1; x < F_height; ++x)
		{
			double normalized = (heatData[k++] - averageCorrelation);
			if( maxCorrelation != averageCorrelation && minCorrelation != averageCorrelation )
			{
				if(normalized >= 0.0) normalized /= (maxCorrelation - averageCorrelation );
				else normalized /= (averageCorrelation - minCorrelation);
			}
			heatMap.push_back(redBlueSpectrum(normalized));
		}   //-1.0 / min / 0 / score / avg / max / 1.0
	}
	return heatMap;
}

color OligomerDisplay::redBlueSpectrum(double i)
{
	return color( (int)(max(0.0, i) * 255), 0, (int)(min(0.0, i) * -255));
}
 
void OligomerDisplay::load_canvas()
{
	pixels.clear();
	height();
	
	//set scaling
	min_score = freq[0][0];
	max_score = freq[0][0];
	for( int h = 0; h < F_height; h++)
	{		
		for(int w = 0; w < F_width; w++)
		{
			if( freq[h][w] < min_score)
				min_score = freq[h][w];
			if( freq[h][w] > max_score)
				max_score = freq[h][w];
		}
	}
			
	//paint scaled pixels
	range = max_score - min_score;		
		
	for( int h = 0; h < F_height; h++)
	{		
		for(int w = 0; w < F_width; w++)
		{
			int grey = static_cast<int>( ((freq[h][w]-min_score)/range) * 255 );
			for(int m = 0; m < widthMultiplier; ++m)
				pixels.push_back( color(grey, grey, grey) );
		}
	}
	//vector<color> chart = calculateBoundaries(pixels, F_width*widthMultiplier, similarityGraphWidth);	
	
	storeDisplay( pixels, F_width*widthMultiplier );
	
	//delete graphBuffer;
	//graphBuffer = new TextureCanvas( chart, similarityGraphWidth );

	upToDate = true;
}

GLuint OligomerDisplay::render()//deprecated
{
	return 0;
}

void OligomerDisplay::freq_map()
{
	//ui->print("OligomerDisplay: ", ++frameCount);
	height();
	const char* genome = sequence->c_str() + nucleotide_start;
	for( int h = 0; h < F_height; h++)
	{
		vector<int> temp_map = vector<int>(F_width, 0);
		int offset = h * Width;
		for(int l = 0; l < Width; l++)
		{	
			int oligIndex = 0;
			for(int c = 0; c < wordLength; ++c)
			{
				oligIndex = oligIndex * 4 + ACGT_num( genome[offset + l + c] );				
			}				
			if( oligIndex >= 0 )
				++temp_map[oligIndex];
		}
		for(int w = 0; w < F_width; w++)//load temp_map into freq
		{
			freq[h][w] = temp_map[w];
		}
	}	
	upToDate = true;
}

int OligomerDisplay::oligNum(string a)
{
	int oligIndex = 0;
	for(int c = 0; c < (int)a.size(); ++c)
	{
		oligIndex = oligIndex * 4 + ACGT_num( a[c] );				
	}
	return oligIndex;
}

int OligomerDisplay::height()
{	
	//Width = ui->widthDial->value();	
	//if(Width < 1) Width = 1;
		
	F_height = (display_size - wordLength ) / Width;

	F_height = max(0, min(400, F_height) );
	
	return F_height;
}

/******SLOTS*****/
string OligomerDisplay::mouseClick(point2D pt)
{
	//range check
	if( pt.x < (int)width()-similarityGraphWidth && pt.x >= 0  )
	{
		pt.x = pt.x / 2;
		int index = pt.y * ui->widthDial->value();
		index = index + nucleotide_start;
		int w = min( 100, ui->widthDial->value() );
		stringstream ss;
		ss << "Dinucleotide: " << pt.x << "  Count: " << freq[pt.y][pt.x] << "\nSequence:"
			<< sequence->substr(index, w);
		ui->print(ss.str().c_str());
	}
	return string();
}

color randomColor()
{
	volatile int r = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int g = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int b = (int)(((float)rand() / RAND_MAX)* 255);
	return color(r, g, b);
}

double OligomerDisplay::correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample)//calculations for a single pixel
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

double OligomerDisplay::correlate(vector<double>& img, int beginA, int beginB, int pixelsPerSample)//calculations for a single pixel
{//correlation will be a value between -1 and 1 representing how closley related 2 sequences are
	//calculation variables!!!  should all be double to prevent overflow
	double N = pixelsPerSample;
	double AVal;		double BVal;
	
	double ARedsum = 0;   //our tuple of color sums
	double BRedsum = 0;   //our tuple of color sums
	double ASquaredRed = 0;  //this is Aij^2
	double BSquaredRed = 0;  //this is Bij^2	
	double ABRed = 0;	//this is A[]*B[]

	for (int k = 0; k < pixelsPerSample; k++)
	{//3 color shades RGB,  2 samples A and B
		//reds
		AVal = img[beginA + k];
		BVal = img[beginB + k];
		ARedsum += AVal;					BRedsum += BVal;
		ASquaredRed += (AVal*AVal);			BSquaredRed += (BVal*BVal);
		ABRed += (AVal * BVal);						
	}   
	
	//calculation time
	double AbarRed = 0;  //A-tuple for color means
	double BbarRed = 0;  //B-tuple for color means
	AbarRed = ARedsum / N;
	BbarRed = BRedsum / N;
	
	double numerator_R = ABRed   - BbarRed   * ARedsum   - AbarRed * BRedsum     + AbarRed   * BbarRed   * N;	
	double denom_R1 = (sqrt(ASquaredRed   - ((ARedsum   * ARedsum)  /N)));
	double denom_R2 = (sqrt(BSquaredRed   - ((BRedsum   * BRedsum)  /N)));
	
	double backup = sqrt(1 - (1/N));//if we have 0 instances of a color it will be / 0  div0
	if(denom_R1 == 0) denom_R1 = backup;
	if(denom_R2 == 0) denom_R2 = backup;

	double answer_R = numerator_R / (denom_R1 * denom_R2);

	return answer_R ;
}



int OligomerDisplay::width()
{
	widthMultiplier = 1;/*
	if(wordLength < 3)
	{
		widthMultiplier = 4;
	}
	else if(wordLength == 3)
	{
		widthMultiplier = 2;
	}*/

	return F_width*widthMultiplier + (F_height+2)*2;
}

/*vector<color> OligomerDisplay::calculateAverageSignature(int begin, int end)
{
	vector<color> avg;
	int regionSize = end - begin;
	for(int x = 0; x < F_width; ++x)
	{
		float total = 0;
		for(int y = begin; y < end && y < (int)freq.size(); ++y)
		{
			total += freq[y][x];
		}
		total = total / regionSize;
		int grey = static_cast<int>( ((total-min_score)/range) * 255 );
		for(int m = 0; m < widthMultiplier; ++m)
			avg.push_back(color( grey, grey, grey));
	}
	
	return avg;
}

void OligomerDisplay::isochores()
{//16839493
	int prevBoundary = 0;
	vector<color> averages;
	for(int i=0; i < (int)boundaryIndices.size(); ++i)
	{
		vector<color> avg = calculateAverageSignature(prevBoundary, boundaryIndices[i]);
		int regionSize = boundaryIndices[i] - prevBoundary;
		for(int k = 0; k < regionSize; ++k)
			averages.insert(averages.end(), avg.begin(), avg.end());
		prevBoundary = boundaryIndices[i];
	}
	
	//loadAveragedCanvas
	delete avgBuffer;
	avgBuffer = new TextureCanvas( averages, F_width* widthMultiplier );
}*/

/*vector<color> OligomerDisplay::calculateBoundaries(vector<color>& img, int row_size, int graphWidth)
{
	//compute average value
	double average = 0.0;
	vector<double> scores;
	for(int i = 0; i+row_size < (int)img.size(); i+=row_size)
	{
		double score = correlate(img, i, i+row_size, row_size);
		average += score;
		scores.push_back( score ); 
	}	
	average = average / (double)scores.size();
	
	//list squared variance from average, and average variance (which is confusing)
	double averageVariance = 0.0;
	for(int i = 0; i+1 < (int)scores.size(); ++i)
	{
		double variant = scores[i] - average;
		variant = variant * variant;// square value
		averageVariance += variant;
	}
	averageVariance /= (double)scores.size();
	double standardDeviation = sqrt(averageVariance);
		
	minimumCorrelation = average - minDeltaBoundary*standardDeviation;
	upperCorrelation = average + minDeltaBoundary*standardDeviation;
	averageCorrelation = average;
}/**/
