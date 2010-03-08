#include "HighlightDisplay.h"
#include "glwidget.h"
#include <sstream>

HighlightDisplay::HighlightDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{	
	hidden = true;
	actionLabel = string("Sequence Highlighter");
	actionTooltip = string("Highlights user specified sequences");
	actionData = actionLabel; 
	
	target = string("AAAAAAAAAAAA");
	percentage_match = 0.8;
	
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
		//if(!upToDate)
			assignColors();
		textureBuffer->display();
	glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void HighlightDisplay::assignColors()
{
	//if(!upToDate)
		calculate();
	
	nucleotide_colors.clear();
	int targetSize = target.size();
	int remainingLength = 0;
	int match_minimum = (int)(255 * percentage_match);
	const char* seq = (sequence->c_str()+nucleotide_start);
	for(int i = 0; i < (int)similarity.size(); i++)
	{
		int grey = static_cast<int>(  float(similarity[i])/targetSize * 255 );//Grey scale based on similarity
		color pixelColor = color(grey, grey, grey);//white to grey
		
		//highlight matches with color
		if(grey >= match_minimum)//count this as a starting position
		{
			remainingLength = targetSize - 1;
			pixelColor = color(255, 0, 0);//red
		}
		else
		{
			if(remainingLength)//trail after a match
			{//green if it matches, blue if it doesn't
				//if((*sequence)[i+nucleotide_start] == target[targetSize - remainingLength])
				if(seq[i] == target[targetSize - remainingLength])
					pixelColor = color(0, 255, 0);//green
				else
					pixelColor = color(0, 0, 255);//blue
					
				remainingLength--;
			}
		}
		nucleotide_colors.push_back( pixelColor );
	}
	loadTextureCanvas();
}

//This calculates how well a region of the genome matches a query sequence 'target' at every nucleotide.  
void HighlightDisplay::calculate()
{
	similarity.clear();
	int targetSize = target.size();
	//int count = 0;
	int start = nucleotide_start;
	const string& seq = (*sequence);
	for( int h = 0; h < display_size && h  < (int)seq.size() - start - (targetSize-1); h++)
	{
		/*//NOTE: This statement is just an optional speed up
		if(seq[start + h] == 'N' || seq[start + h] == 'n' )
		{//check the first and last of the reference string
			//similarity.push_back(0);
		}
		else
		{*/
			unsigned short int score = 0;
			int start_h = start + h;
			for(int l = 0; l < targetSize; l++)
			{		//if( target[l] == 'N' || ...
				if(seq[start_h + l] == target[l])//this is the innermost loop.  This line takes the most time
					++score;
			}
			similarity.push_back(score);
			//if( score >= targetSize*percentage_match )//
			//	++count;//
		//}
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
	upToDate=false;
	invalidate();
}
