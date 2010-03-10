#include "HighlightDisplay.h"
#include "glwidget.h"
#include <sstream>
#include <algorithm>

HighlightDisplay::HighlightDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{	
	hidden = true;
	actionLabel = string("Sequence Highlighter");
	actionTooltip = string("Highlights user specified sequences");
	actionData = actionLabel; 
	
	target = string("AAAAAAAAAAAA");
	percentage_match = 0.8;
	frameNumber = 0;
	
}

HighlightDisplay::~HighlightDisplay(){
    glDeleteLists(display_object, 1);
}

GLuint HighlightDisplay::render()
{
	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushMatrix();
	glScaled(1,-1,1);
		if(!upToDate)
			assignColors();
		textureBuffer->display();
	glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void HighlightDisplay::assignColors()
{
	if(!upToDate)
		calculate();

	nucleotide_colors.clear();
	int targetSize = target.size();
	int remainingLength = 0;
	int match_minimum = (int)(255 * percentage_match);
	const char* seq = (sequence->c_str()+nucleotide_start);
	int offset = 0;
	for(int i = 0; i < (int)similarity.size(); i+=scale)
	{
		vector<unsigned short int>::iterator bestMatch = max_element(similarity.begin()+i, similarity.begin()+i+scale);
		short int bestScore = *bestMatch;
		int grey = static_cast<int>(  float(bestScore)/targetSize * 255 );//Grey scale based on similarity
		color pixelColor = color(grey, grey, grey);//white to grey
		
		//highlight matches with color
		if(grey >= match_minimum)//count this as a starting position
		{
			offset = distance(similarity.begin()+i, bestMatch);
			remainingLength = targetSize - 1;
			pixelColor = color(255, 0, 0);//red
		}
		else
		{
			if(remainingLength >= 1 && remainingLength >= scale)//trail after a match
			{//green if it matches, blue if it doesn't
				if(seq[i+offset] == target[targetSize - remainingLength - offset])
					pixelColor = color(0, 255, 0);//green
				else
					pixelColor = color(0, 0, 255);//blue
					
				remainingLength = max(0, remainingLength - scale);
			}
		}
		nucleotide_colors.push_back( pixelColor );
	}
	loadTextureCanvas();
}

//This calculates how well a region of the genome matches a query sequence 'target' at every nucleotide.  
void HighlightDisplay::calculate()
{
	//glWidget->print("Calculate:", ++frameNumber);
	similarity.clear();
	int targetSize = target.size();
	//int count = 0;
	int start = nucleotide_start;
	const string& seq = (*sequence);
	for( int h = 0; h < display_size && h  < (int)seq.size() - start - (targetSize-1); h++)
	{
			unsigned short int score = 0;
			int start_h = start + h;
			for(int l = 0; l < targetSize; l++)
			{
				if(seq[start_h + l] == target[l])//this is the innermost loop.  This line takes the most time
					++score;
			}
			similarity.push_back(score);
	}	
	upToDate = true;
//	stringstream outStream;//
//	outStream << count << " matches to " << target << " at " << percentage_match * 100 << "% similarity" << endl;
//	glWidget->print(outStream.str());
}

void HighlightDisplay::setHighlightSequence(const QString& high_C)
{
	string high = high_C.toStdString();
	for(int l = 0; l < (int)high.size(); l++)
		if(high[l] >= 97 && high[l] <= 122) high[l] -= 32;//cast to uppercase

	target = high;
	similarity.clear();
	//similarity.resize(display_size, 0);//could be commented out if you use push_back instead	
	invalidate();
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
