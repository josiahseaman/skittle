#include "HighlightDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <algorithm>
#include <QtGui/QFrame>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

HighlightDisplay::HighlightDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{	
	hidden = true;
	actionLabel = string("Sequence Highlighter");
	actionTooltip = string("Highlights user specified sequences");
	actionData = actionLabel; 
	
	targets.push_back(string("AAAAAAAAAAAA"));
	//targets.push_back(string("TTTTTTTTTTTT"));
	percentage_match = 0.8;
	frameCount = 0;
	
}

HighlightDisplay::~HighlightDisplay(){
    glDeleteLists(display_object, 1);
}

QFrame* HighlightDisplay::settingsUi()
{	
    settingsTab = new QFrame();    
    settingsTab->setWindowTitle(QString("Sequence Highlighter Settings"));
	formLayout = new QFormLayout;
	formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
	settingsTab->setLayout(formLayout);
	
    activeSeqEdit = new QLineEdit(settingsTab);
    activeSeqEdit->setText("AAAAAAAAAAAA");
    QSpinBox* similarityDial = new QSpinBox(settingsTab);
    similarityDial->setValue(80);
    reverseCheck = new QCheckBox("Search Reverse Complement", settingsTab);
    reverseCheck->setChecked(true);
    QPushButton* addButton = new QPushButton("Add a New Sequence", settingsTab);
    
    formLayout->addRow(reverseCheck);
    formLayout->addRow("Minimum Percent Similarity:", similarityDial);
    formLayout->addRow(addButton);
    formLayout->addRow("Sequence 1:", activeSeqEdit);
    //activeSeqEdit->setMinimumWidth(400);
	
	connect( addButton, SIGNAL(clicked()), this, SLOT(addNewSequence()));
	
	connect( activeSeqEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setHighlightSequence(const QString&)));
	connect(this, SIGNAL(highlightChanged(const QString&)), activeSeqEdit, SLOT(setText(const QString&)) );
	
    connect( similarityDial, SIGNAL(valueChanged(int)), this, SLOT(setPercentSimilarity(int)));     	
	connect( reverseCheck, SIGNAL(released()), this, SLOT(invalidate()));
	return settingsTab;
}

void HighlightDisplay::display()
{
	checkVariables();
	glPushMatrix();
	glScaled(1,-1,1);
		if(!upToDate)
		{
			vector<vector<int> > results;
			for(int i = 0; i < (int)targets.size(); i++)
				results.push_back( identifyMatches(targets[i]) );
			//vector<int> forward = identifyMatches(target);
			//vector<int> reverse;
			//if(reverseCheck->isChecked())
			//	reverse = identifyMatches(reverseComplementTarget);
			//combine(forward, reverse);
			combine( results );
			loadTextureCanvas();
			upToDate = true;
		}
		if( !nucleotide_colors.empty())
			textureBuffer->display();
	glPopMatrix();
}

GLuint HighlightDisplay::render()//deprecated
{
	GLuint list = glGenLists(1);
    return list;
}

vector<int> HighlightDisplay::identifyMatches(string find)
{
	vector<unsigned short int> scores = calculate(find);

	vector<int> pixels;
	int findSize = find.size();
	int remainingLength = 0;
	int match_minimum = (int)(255 * percentage_match);
	const char* seq = (sequence->c_str()+nucleotide_start);
	int offset = 0;
	for(int i = 0; i < (int)scores.size(); i+=scale)
	{
		vector<unsigned short int>::iterator bestMatch = max_element(scores.begin()+i, scores.begin()+i+scale);
		short int bestScore = *bestMatch;
		int grey = static_cast<int>(  float(bestScore)/findSize * 255 );//Grey scale based on similarity
		int pixelColor = grey;//white to grey
		
		//highlight matches with color
		if(grey >= match_minimum)//count this as a starting position
		{
			offset = distance(scores.begin()+i, bestMatch);
			remainingLength = findSize - scale;
			pixelColor = 259;
		}
		else{
			if(remainingLength >= 1 && remainingLength >= scale)//trail after a match
			{//green if it matches, blue if it doesn't
				if(seq[i+offset] == find[findSize - remainingLength])
					pixelColor = 259;//green
				else
					pixelColor = 257;//blue
					
				remainingLength = max(0, remainingLength - scale);
			}
		}
		pixels.push_back( pixelColor );
	}
	return pixels;
}

//This calculates how well a region of the genome matches a query sequence 'find' at every nucleotide.  
vector<unsigned short int> HighlightDisplay::calculate(string find)
{
	vector<unsigned short int> scores;
	int findSize = find.size();
	//int count = 0;
	int start = nucleotide_start;
	const string& seq = *sequence;
	for( int h = 0; h < display_size && h  < (int)seq.size() - start - (findSize-1); h++)
	{
			unsigned short int score = 0;
			int start_h = start + h;
			for(int l = 0; l < findSize; l++)
			{
				if(seq[start_h + l] == find[l])//this is the innermost loop.  This line takes the most time
					++score;
			}
			scores.push_back(score);
	}	
//	stringstream outStream;//
//	outStream << count << " matches to " << find << " at " << percentage_match * 100 << "% similarity" << endl;
//	ui->print(outStream.str());
	return scores;
}

void HighlightDisplay::combine(vector< vector<int> >& results)
{
/***HIGHLIGHT COLOR LEGEND***/
	//VALUE		MEANING		COLOR
	//0-255 	no match	grey scale
	//256 		buffer 		(white)
	//257		R mismatch	yellow   1 + 256
	//258		F mismatch	blue     2 + 256
	//259		R match		purple   3 + 256
	//260		F match		green    4 + 256
	
	//ensure that forward and reverse are the same length
	//unsigned int size = min( forward.size(), reverse.size() );
	//forward.resize(size);
	//reverse.resize(size);	
	nucleotide_colors.clear();
	
	//ensure they're all the same length
	int nSequences = results.size();
	int length = results[0].size();
	vector< vector<int>::iterator > iterators;
	for(unsigned int i = 0; i < results.size(); ++i)
	{
		if( length != results[i].size() )
		{
			results[i].resize(length, 0 );
			//ui->print("ERROR: Highlight Search strings were different lengths.");
			//return;
		}
		iterators.push_back( results[i].begin() );
	}
	
	color c = color(0,0,0);
	for(int i = 0; i < length; i++)
	{
		int score = 0;
		for(int k = 0; k < nSequences; ++k)
		{
			score = max(score, results[k][i]);//*(iterators[k]));
			//score = *(iterators[k]);
			//++iterators[k];
		}
			
		if(score < 257)
		{
			 c = color(score, score, score);
		}
		else{
			if( score == 257) c = color(255, 215, 0);	//257		R mismatch	yellow   1 + 256
			if( score == 258) c = color(0, 0, 255);		//258		F mismatch	blue     2 + 256
			if( score == 259) c = color(245, 0, 0);		//259		R match		purple   3 + 256
			if( score == 260) c = color(0, 255, 0);		//260		F match		green    4 + 256
		}
		nucleotide_colors.push_back( c );
	}
}
/*void HighlightDisplay::combine(vector<int>& forward, vector<int>& reverse)
{
///
	//VALUE		MEANING		COLOR
	//0-255 	no match	grey scale
	//256 		buffer 		(white)
	//257		R mismatch	yellow   1 + 256
	//258		F mismatch	blue     2 + 256
	//259		R match		purple   3 + 256
	//260		F match		green    4 + 256
	
	//ensure that forward and reverse are the same length
	//unsigned int size = min( forward.size(), reverse.size() );
	//forward.resize(size);
	//reverse.resize(size);	
	nucleotide_colors.clear();
	
	color c = color(0,0,0);
	for(unsigned int i = 0; i < forward.size(); i++)
	{
		int score = 0;
		if(!reverse.empty())
			score = max(forward[i]+1, reverse[i]);
		else 
			score = forward[i]+1;
			
		if(score < 257)
		{
			 c = color(score, score, score);
		}
		else{
			if( score == 257) c = color(255, 215, 0);	//257		R mismatch	yellow   1 + 256
			if( score == 258) c = color(0, 0, 255);		//258		F mismatch	blue     2 + 256
			if( score == 259) c = color(245, 0, 0);		//259		R match		purple   3 + 256
			if( score == 260) c = color(0, 255, 0);		//260		F match		green    4 + 256
		}
		nucleotide_colors.push_back( c );
	}
}
*/
inline char complement(char a)
{
	if(a == 'A') return 'T';
	if(a == 'C') return 'G';
	if(a == 'G') return 'C';
	if(a == 'T') return 'A';
	return 'N';
}

string HighlightDisplay::reverseComplement(string original)
{
	string rc;
	int size = original.size();
	rc.resize(size, 'N');
	for(int x = 0; x < size; ++x)
	{					
		rc[x] = complement(original[size-x-1]);
	}
	return rc;
}

void HighlightDisplay::setHighlightSequence(const QString& high_C)
{
	string high = high_C.toStdString();
	if( !targets.empty() && high.compare(targets[targets.size()-1]) == 0)
		return;
		
	for(int l = 0; l < (int)high.size(); l++)
		if(high[l] >= 97 && high[l] <= 122) high[l] -= 32;//cast to uppercase

	//targets.clear();
	targets[targets.size()-1] = high;
//	if(reverseCheck->isChecked())
	//	targets.push_back( reverseComplement(high) );
	//ui->print("Number of strings currently highlighted: ", targets.size());
	
	
	QString copy(high.c_str());
	emit highlightChanged(copy);
	
	invalidate();
}

void HighlightDisplay::addNewSequence()
{	
	disconnect( activeSeqEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setHighlightSequence(const QString&)));	
	disconnect(this, SIGNAL(highlightChanged(const QString&)), activeSeqEdit, SLOT(setText(const QString&)) );
	
    activeSeqEdit = new QLineEdit(settingsTab);	
	std::stringstream ss1;
	ss1 << "Sequence " << targets.size()+1;//Sequence number
    formLayout->addRow(ss1.str().c_str(), activeSeqEdit);
	targets.push_back( string() );
	
	connect( activeSeqEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setHighlightSequence(const QString&)));
	connect(this, SIGNAL(highlightChanged(const QString&)), activeSeqEdit, SLOT(setText(const QString&)) );
}

void HighlightDisplay::setPercentSimilarity(int percentile)
{
	double newVal = (float)percentile / 100.0;
	if( percentage_match != newVal )
	{
		percentage_match = newVal;
		invalidate();
	}
}
