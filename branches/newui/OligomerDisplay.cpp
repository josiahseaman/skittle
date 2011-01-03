#include "OligomerDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <math.h>
#include <QDoubleSpinBox>
#include <algorithm>

double valueForN = -2.0;

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
*** these connections should be placed in glWidget's constructor.    
* 
* See note in AbstractGraph::createConnections.
* ***/
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
		freq = vector< vector<double> >();
		for(int i = 0; i < 400; i++)
		{
			freq.push_back( vector<double>(F_width, 0.0) );
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
	}
	width();
	glPushMatrix();
		glScaled(1,-1,1);
		textureBuffer->display();
		glTranslated(F_width*widthMultiplier + 2, 0, 0);
		heatMapBuffer->display();
		glTranslated(F_height + 2, 0, 0);
		correlationBuffer->display();
	glPopMatrix();
}

void OligomerDisplay::calculateHeatMap()
{	
	scores.clear();
	for(int y = 0; y < F_height; ++y)
	{	
		for(int x = y+1; x < F_height; ++x)
		{
			int row = F_width*widthMultiplier;/**/
			double score = correlate(freq[y], freq[x]);/*/ //pixels, x*row, y*row, row
			double score = spearmanCorrelation(freq[y], freq[x]);/**/
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
			vector<double> input1 = copyVectorRange(filledScores,  y*row, row); //row
			vector<double> input2 = copyVectorRange(rotated, x*row, row); //column
			/**/
			double score = correlate(input1, input2); /*/ // 0, row, row
			double score = spearmanCorrelation(input1, input2);/**/
			correlationScores.push_back(score);
		}	
	}
	
	vector<color> heatMap = colorNormalized(correlationScores);
	delete correlationBuffer;
	correlationBuffer = new TextureCanvas( heatMap, F_height );	
}

vector<double> OligomerDisplay::copyVectorRange(vector<double>& stuff, int index, int length)
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
	
	vector<double> sortedScores = heatData;
	sort(sortedScores.begin(), sortedScores.end());

	double maxCorrelation = sortedScores[sortedScores.size()-1];//*(max_element(heatData.begin(), heatData.end()));
	double minCorrelation = sortedScores[0];
	int i = 0; 
	while(minCorrelation == valueForN && i < sortedScores.size()-1)
			minCorrelation = sortedScores[++i];//scan for a value other than valueForN
	double medianCorrelation = sortedScores[ (sortedScores.size()-i)/2 + i ];
	
	
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
			double val = heatData[k++];
			if(val != valueForN)
			{
				double normalized = (val - medianCorrelation);
				if( maxCorrelation != medianCorrelation && minCorrelation != medianCorrelation )
				{
					if(normalized >= 0.0) normalized /= (maxCorrelation - medianCorrelation );
					else normalized /= (medianCorrelation - minCorrelation);
				}
				heatMap.push_back(redBlueSpectrum(normalized));
			}
			else heatMap.push_back(color(150,150,150));
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
	min_score = freq[0][0];//TODO: if value is -1 this will throw everything off
	max_score = freq[0][0];
	for( int h = 0; h < F_height; h++)
	{		
		for(int w = 0; w < F_width; w++)
		{
			if( freq[h][w] < min_score)
				if(freq[h][w] >= 0)
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
			double grey = freq[h][w];
			if( grey >= 0)
			{
				grey = static_cast<int>( ((grey-min_score)/range) * 255 );
			}
			else grey = 150;
			for(int m = 0; m < widthMultiplier; ++m)
				pixels.push_back( color(grey, grey, grey) );
		}
	}
	
	storeDisplay( pixels, F_width*widthMultiplier );

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
		
		if(genome[offset] != 'N' && genome[offset+Width] != 'N')
		{
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
		else
		{
			for(int w = 0; w < F_width; w++)
			{
				freq[h][w] = valueForN;
			}
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

//Pearson Correlation
double OligomerDisplay::correlate(vector<double>& apples, vector<double>& oranges)
{
	int pixelsPerSample = (int) min(apples.size(), oranges.size());
	double N = pixelsPerSample;
	double AVal;		double BVal;
	
	double Asum = 0;   
	double Bsum = 0;   
	double ASquared = 0;  //this is Aij^2
	double BSquared = 0;  //this is Bij^2	
	double AB = 0;	//this is A[]*B[]

	for (int k = 0; k < pixelsPerSample; k++)
	{
		AVal = apples [k];
		BVal = oranges[k];
		if(AVal == valueForN || BVal == valueForN)
		{
			--N;
		}
		else
		{
			Asum += AVal;                   Bsum += BVal;
			ASquared += (AVal*AVal);        BSquared += (BVal*BVal);
			AB += (AVal * BVal);						
		}
	}  
	if( N <= 0)
		return valueForN;//no data to report on
		
	double Abar = 0;  
	double Bbar = 0;
	Abar = Asum / N;
	Bbar = Bsum / N;
	
	double numerator = AB   - Bbar   * Asum   - Abar * Bsum     + Abar   * Bbar   * N;	
	double denom_1 = sqrt(ASquared   - ((Asum   * Asum)  /N));
	double denom_2 = sqrt(BSquared   - ((Bsum   * Bsum)  /N));

	double answer = numerator / (denom_1 * denom_2);

	return answer;
}

bool orderByValue(point A, point B)
{
	return (A.x < B.x);
}

bool orderByPosition(point A, point B)
{
	return (A.y < B.y);
}

void averageRanksOfEqualValue(vector<point>& temp, int& i)
{
	int startingRank = i;
	int endingRank = i;
	while(temp[i].x == temp[i+1].x)//detect equal values and average ranks
	{
		endingRank = i+1;
		++i;
		if(i < (int)temp.size()-1)
			break;
	}
	if( startingRank != endingRank)
	{
		double averageRank = (startingRank + endingRank) / 2.0;
		for(int k = startingRank; k <= endingRank; ++k)
		{
			temp[k].z = averageRank;
		}
	}
}

void OligomerDisplay::assignRanks(vector<point>& temp)
{
	for(int i = 0; i < (int)temp.size()-1; ++i)
	{
		temp[i].z = i;
		//averageRanksOfEqualValue(temp, i);
	}
	temp[temp.size()-1].z = temp.size()-1;//assign last position
}

double OligomerDisplay::spearmanCorrelation(vector<double>& apples, vector<double>& oranges)
{
	vector<point> tempA;
	vector<point> tempB;
	int valsPerSample = min(apples.size(), oranges.size());
	//create triplets of numbers  <x,y> = <value, original position, rank>
	for(int i = 0; i < valsPerSample; ++i)
	{
		tempA.push_back(point(apples[i], i, 0 ) );
		tempB.push_back(point(oranges[i], i, 0 ) );
	}
	//sort pairs = sort by value...
	sort(tempA.begin(), tempA.end(), orderByValue);
	sort(tempB.begin(), tempB.end(), orderByValue);
	
	//assign ranks = count upwards, detect equal values
	assignRanks(tempA);
	assignRanks(tempB);

	//resort back to original order = sort by original position
	sort(tempA.begin(), tempA.end(), orderByPosition);
	sort(tempB.begin(), tempB.end(), orderByPosition);

	vector<double> ranks1;
	for(int i = 0; i < (int)tempA.size(); ++i)
		ranks1.push_back( tempA[i].z );
	
	vector<double> ranks2;
	for(int i = 0; i < (int)tempB.size(); ++i)
		ranks2.push_back( tempB[i].z );	
		
	//Pearson Correlation on ranks
	return correlate(ranks1, ranks2);	
}
	
int OligomerDisplay::width()
{
	widthMultiplier = 1;

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


//start: 20541380  width: 479304
