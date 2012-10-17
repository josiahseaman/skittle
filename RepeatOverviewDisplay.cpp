//RepeatOverviewDisplay.cpp
#include <sstream>
#include "RepeatOverviewDisplay.h"
#include "glwidget.h"

/** ******************************  Graph Class
Repeat Overview is the next step beyond RepeatMap.  As RepeatMap is a summary
of NucleotideDisplay, so RepeatOverview is a summary of RepeatMap.  Each pixel
of RepeatOverview represents the "best match" from a RepeatMap row.  The size
of string that it tries to match is based on the ui->scaleDial variable.  For
example, at scale 10 each pixel represent 10bp.  The result is the equivalent
of setting RepeatMap at width 10 (10bp per line) and finding the highest score
for that line.  The highest score will be a single number between 1 and 250
(the default scan range).  That number is the frequency of the tandem reapeat.
The frequency is then represented with a hue mapped along a rainbow: 1 = blue,
250 = cyan.  Of course not all sequences are tandem repeats.  The brightness of
the pixel indicates the highest score, making non-repetitive areas dark colored.

The fundamental difficulty in coding RepeatOverview is that it is designed to
work best at large scales, whereas all other Graphs start at scale 1 and work
up.  RepeatOverview at scale 1 is not terribly meaningful.  ((I suppose each pixel
would represent the number of nucleotides until that nucleotide was repeated.
So given the sequence ACCAGG... the answer would be 31--1-...))  RepeatOverview
is also very CPU intensive.  The current optimization packs 4 letters into one byte
then does comparisons in groups of four.  This requires a lot of bit level logic
and forces the scale to be a multiple of four.  This means that RepeatOverview
is the only Graph mode that must push values back to the global ui, causing update
challenges.  Example: RepeatOverviewDisplay::toggleVisibility()

It is important to realize that RepeatOverview is the most difficult because it is
the most ambitious, not because it is badly written.  The computational complexity
of the base problem is higher than anything else in Skittle:
Big(O) = N * Scale * 250
N = number of pixels
250 = default scan range = number of RepeatMap comparisons
This means that regardless of how "zoomed out" the user is, RepeatOverview computes
over every letter of the file in the visible region.  This is necessary in order to
maintain the ability to detect 3bp frequency repeats at the scale of a whole chromosome.
So for Human Chromosome 1 which is 246 million letters, Skittle performs
246 million * 250 = 61 billion equivalence tests in order to display a single frame
of the RepeatOver, updated in real-time.  THAT is why it's laggy.

Performance Optimizations: Since there are only 4 possible nucleotides, RepeatOverview
packs 4bp into the 8 bits of a byte.  This is stored in unsigned char* packSeq.  This
garners a 4x speed increase.  Furthermore, the comparisons are done using operations
over the size of one long int, which is at least 64 bits.  This is at least another
16x increase in performance, however it incurs overhead.  In order to work with these
packed data types the header file contains a set of methods labeled "Optimized Long Int Accessors"
and "Optimized Packed Sequence Methods".

The bit logic in methods like countMatchesShort() bears explaining.  With four possibilities,
a nucleotide take up two bits.  With one bit words, the number of 1's in an XOR will tell you
the number of differences between two strings.  However, with nucleotides an XOR result of
11, 10, and 01 all count as 1 difference.  This means that you have to look at 2 bits at a time
instead of simply counting.  So 1101 is 2 differences, 0110 is 2 differences and 0011 is one
difference.

Development:
*Add text to the spectrum legend.
*Make it useful at scale = 1.  Issue #33
*Make the CPU optimization less buggy.
*Make the scaleDial feedback for multiples of 4 unnecessary.
*************************************/

RepeatOverviewDisplay::RepeatOverviewDisplay(UiVariables* gui, GLWidget* gl)
    :NucleotideDisplay(gui, gl)
{
    hidden = true;
    charPerIndex = 4;
	sequence = NULL;
    packSeq = NULL;
	countTableShort = NULL;
	countTableChar = NULL;
	pSeqSize = 0;

	calcMatchTable();
	//packSequence is called by setSequence()
	
	actionLabel = string("Repeat Overview");
	actionTooltip = string("Color by the best alignment offset");
	actionData = actionLabel; 
}

void RepeatOverviewDisplay::display()//TODO: Delete this whole method since it is redundant with AbstractGraph
{
	checkVariables();
    glPushMatrix();
	glScaled(1,-1,1);
	
		if(!upToDate)
			loadTexture();
		textureBuffer->display();

	glPopMatrix();
}

void RepeatOverviewDisplay::loadTexture()
{
	/**/
    qDebug() << "RepeatOverviewDisplay::load: " << ++frameCount;
	int s = ui->scaleDial->value();
	if(s % 4 != 0)
	{
		ui->print("Warning: The SCALE on Repeat Overview should be set to a multiple of 4.");
	}
	//s = max(4, (s / 4) * 4);//enforces scale is a multiple of 4
	//ui->scaleDial->setValue(s);
    /* /
    if(!align->hidden)
    {
        int scale = ui.scaleDial->value();
        int rem = scale % 4;
        scale = scale - rem;//enforces scale is a multiple of 4
        scale = max(4, scale);
        if(scale != ui.scaleDial->value())
        {
            if(rand() % 10 != 1)
            {
                ui.print("GLWidget::paintGL ", scale);
                return;
            }
        }
    }*/

    qDebug() << "Width: " << ui->widthDial->value() << "\nScale: " << ui->scaleDial->value() << "\nStart: " << ui->startDial->value();
	vector<color> alignment_colors;
    int end = max(1, (ui->startDial->value() + current_display_size()) - 251);
    int tempScale = ui->scaleDial->value();
    for(int i = ui->startDial->value(); i < end; i += tempScale)
		alignment_colors.push_back( simpleAlignment(i) );

	storeDisplay( alignment_colors, width());

	upToDate = true;
}

GLuint RepeatOverviewDisplay::render()
{
	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushMatrix();
	glScaled(1,-1,1);
	
		if(!upToDate)
			loadTexture();
		textureBuffer->display();

	glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void RepeatOverviewDisplay::VLRcheck(vector<point> matches)//alternative to loadTexture
{
	/*int s = ui->scaleDial->value();
	s = max(4, (s / 4) * 4);//enforces scale is a multiple of 4
	ui->scaleDial->setValue(s);
	checkVariables();*/
	//return if changed?

	vector<color> alignment_colors;
    int end = max(1, (ui->startDial->value() + current_display_size()) - 251);
    int tempScale = ui->scaleDial->value();
    for(int i = ui->startDial->value(); i < end; i += tempScale)
		alignment_colors.push_back( simpleAlignment(i) );

	mergeMatches( alignment_colors, matches );

	storeDisplay( alignment_colors, width());

	upToDate = true;
}

void RepeatOverviewDisplay::mergeMatches(vector<color>& original, vector<point>& vlr)
{
	int start = 250 / ui->scaleDial->value();
	start = max( 10, start);
    int tempScale = ui->scaleDial->value();
    for( int i = 0; i < (int)vlr.size(); ++i)
	{
		if((vlr[i].y > start) && (vlr[i].x > .7))
		{
            for(int k = i * width(); k < (i+1) * width() && k < (int)original.size(); ++k)
			{
				//original[k] = (original[k]/3) + (alignment_color( vlr[i].x*scale, vlr[i].y)*2.0/3.0);// * scale 
                original[k] = alignment_color( vlr[i].x*tempScale, vlr[i].y);
			}
		}
	}
}

void RepeatOverviewDisplay::displayLegend(float canvasWidth, float canvasHeight)
{
	/** /
	glPushMatrix();
		glTranslated(0,40,0);
	vector<color> paintIt;
	for(int y = 0; y < 10; y++)
	{
	for(int i = 0; i < 250; i++)
	{
				paintIt.push_back(alignment_color(scale, i));//spectrum(i/255.0);//ui->scaleDial->value()
	}
	}
	TextureCanvas paint = TextureCanvas( paintIt, 250);
	paint.display();
	
	glPopMatrix();
	/*/
	glPushMatrix();
		glTranslated(0,-canvasHeight,1);//
		for(int i = 0; i < 250; i++)
		{
			glPushMatrix();
				glTranslated(i,4,0);
                color c = alignment_color(ui->scaleDial->value(), i);//spectrum(i/255.0);//ui->scaleDial->value()
				glScaled(1,10,1);//*(c.b / 125.0)
	    		glColor3d(c.r /255.0, c.g /255.0, c.b /255.0); 
	    	    glBegin(GL_QUADS);
			        glVertex3d(0, .5, 0);
			        glVertex3d(-1, .5, 0);
			        glVertex3d(-1, -.5, 0);
			        glVertex3d(0, -.5, 0);
			    glEnd();
			glPopMatrix();
		}
		//TextRender textOutput = TextRender();
		glColor3d(0,0,0);
		//textOutput.range_labels(1, 250, point(0.5,2,.3), point(250.5,2,.3), 16);
		glColor3d(1,1,1);
		//textOutput.range_labels(1, 250, point(0,1.5,.5), point(250,1.5,.5), 16);
	glPopMatrix();
	/**/
}


color RepeatOverviewDisplay::alignment_color(int score, int frequency)
{
	color c = glWidget->spectrum((double)(frequency) / 250.0);//
	color black = color(0,0,0);
    c = interpolate(black, c, score / float(ui->scaleDial->value()));
	
	return c;
}

color RepeatOverviewDisplay::interpolate(color p1, color p3, double progress)//progress goes from 0.0 p1  to 1.0 p2
{
	double inverse = 1.0 - progress;
	int x2 = (int)(p1.r * inverse + p3.r * progress + .5);
	int y2 = (int)(p1.g * inverse + p3.g * progress + .5);
	int z2 = (int)(p1.b * inverse + p3.b * progress + .5);
	return color(x2, y2, z2);
}

int RepeatOverviewDisplay::countMatchesShort(unsigned short int bits)
{
	int c = 0;
	for(int i = 0; i < (int)(sizeof(unsigned short int) * 4); ++i)
	{
		if( ((bits >> (i*2)) & 3) == 0)// 3 = 00011 look at last two bits
			++c;
	}
	return c;
}

int RepeatOverviewDisplay::countMatchesChar(unsigned char bits)
{
	int c = 0;
	for(int i = 0; i < 4; ++i)
	{
		if( ((bits >> (i*2)) & 3) == 0)// 3 = 00000011 look at last two bits
			++c;
	}
	return c;
}

void RepeatOverviewDisplay::calcMatchTable()
{
	countTableShort = new int[65536];
	for(unsigned short int index = 0; true; ++index)
	{
		countTableShort[index] = countMatchesShort(index);
		if(index == 65535)//last possible index: we can't check for this beforehand because of overflow
			break;
	}
	
	countTableChar = new int[256];
	for(unsigned short int index = 0; ; ++index)
	{
		countTableChar[index] = countMatchesChar(index);
		if(index == 255)//last possible index: we can't check for this beforehand because of overflow
			break;
	}
}

void RepeatOverviewDisplay::normalPack(const string* seq)
{
	if(packSeq)
		delete [] packSeq;
	packSeq = new unsigned char[seq->size() / charPerIndex] ;
	pSeqSize = seq->size() / charPerIndex;
	
	for(unsigned int i = 0; i+charPerIndex < seq->size(); i+=charPerIndex)
	{
		unsigned char num = 0;		
		for(unsigned int n = 0; n < charPerIndex; ++n)
		{
			// A 00 C = 01 G = 10 T = 11
			num = num << 2;
			if((*seq)[i + n] == 'C')
				num += 1;
			else if((*seq)[i + n] == 'G')
				num += 2;
			else if((*seq)[i + n] == 'T')
				num += 3;
		}
 		packSeq[i/charPerIndex] = num;
	}
	//cout << "Finished packing bits" << endl;	
}

void RepeatOverviewDisplay::shiftMask(char* str, int size)
{
	for(int i = size -1; i != -1; --i)
	{
		if(i != size-1)
			str[i+1] |= str[i] << 6;//add the remainder to the following letter  11000000
			
		if(i != 0 && str[i-1] << 6 == 0)
			str[i] = ((unsigned char)str[i]) >> 2;//00111111 create a space in front
		else
			str[i] = str[i] >> 2;
	}
	if( str[0] == 0)
		str[0] = 192;//the bit mask should start growing from 0
}

void RepeatOverviewDisplay::shiftString(unsigned char* str, int size)
{
	str[size-1] = str[size-1] >> 2;
	for(int i = size -2; i != -1; --i)
	{
		str[i+1] |= str[i] << 6;//add the remainder to the following letter  11000000
		str[i] = str[i] >> 2;//00111111 create a space in front
	}	
}

color RepeatOverviewDisplay::simpleAlignment(int index)
{
	if(packSeq[index/4] == 0)
		return color(0,0,0);
		
	//scale % 4 == 0 always
    int reference_size = ui->scaleDial->value() / 4 + 2;//sequence bytes = scale / 4.  1 byte of padding for shifts. 1 byte for sub_index.
    int bitmask_size = reference_size + sizeof(long int);
	int pack_index = index / 4;
	int sub_index = index % 4;
	int max_score = 0;
	int qualifying = 0;
	int best_freq = 251;
	unsigned char reference[reference_size];// = packSeq + index/4;
	char bitmask[bitmask_size];
	for(int i = 0; i < reference_size; ++i)
		reference[i] = packSeq[i+pack_index];//copy values

    int string_end = ui->scaleDial->value()/4;
	for(int i = 0; i < bitmask_size; ++i)
	{
		if( i < string_end)
			bitmask[i] = 0;
		else
			bitmask[i] = 255;
	}		
	if(sub_index)
	{
		if(sub_index == 1)		bitmask[0] = 192;
		else if(sub_index == 2)	bitmask[0] = 240;
		else if(sub_index == 3)	bitmask[0] = 252;
		
		if(sub_index == 1)		bitmask[string_end] = 63;
		else if(sub_index == 2)	bitmask[string_end] = 15;
		else if(sub_index == 3)	bitmask[string_end] = 3;
	
	}
	else{
		--string_end;
	}	
	for(int i = string_end + 1; i < bitmask_size; ++i)
		bitmask[i] = 255;//fill the tail with 1's

	
	for(int frame = 0; frame < 4; ++frame) // 4 read frames.  Step Size = 2 bits
	{
		unsigned char* target = packSeq + pack_index;//a pointer into sequence.  Long int read frame
		for(int offset = 0; offset < 62; ++offset)// 62 * 4 = 248
		{
			int score = 0;
			for(int position = 0; position < reference_size; position+=sizeof(long int))
			{
				unsigned long int diff = (*((unsigned long int*)(reference + position))) ^ (*((unsigned long int*)(target + position)));
				diff = diff | *((unsigned long int*)(bitmask + position));
				score += countTableShort[ (unsigned short int)diff ];//NOTE: This inherently assumes long = short*2
				score += countTableShort[ (unsigned short int)(diff >> 16) ];//and that sizeof(long int)*8 == 32
			}
			if(score >= qualifying)
			{
				if(offset || frame)//offset = 0 will always come through because of the loop
				{//it's more important to ensure we don't incur a check on every offset
					int current_offset = offset * 4 + frame;
					if(current_offset < best_freq)
					{
						max_score = score;
						qualifying = max(0, (int)(floor(score / 1.1)));
						best_freq = current_offset;	
					}
					else if(score >= (int)(ceil(max_score * 1.1)))
					{
						max_score = score;
						qualifying = max(0, (int)(floor(score / 1.1)));
						best_freq = current_offset;							
					}
						
				}
			}

			++target;//increment the target to the next offset
		}
		shiftString(reference, reference_size);//shift two bits to the left
		shiftMask(bitmask, bitmask_size);
	}
	if(best_freq == 0) best_freq = 1;
	//max_score = scale;
	return alignment_color(max_score, best_freq);
	//return color(240,240,17);
}

void RepeatOverviewDisplay::setSequence(const string* seq)
{
	sequence = seq;	
	normalPack(seq);
}

void RepeatOverviewDisplay::changeScale(int s)
{
    ui->print("RepeatOverviewDisplay::changeScale ", s);
    ui->changeScale(s);
}

void RepeatOverviewDisplay::toggleVisibility()
{
	if(hidden)
    {//TODO:change this to use an internal scale without trying to influence the global value
		int s = ui->scaleDial->value();
		s = max(4, (s / 4) * 4);//enforces scale is a multiple of 4
        ui->changeScale(s);
	}
	AbstractGraph::toggleVisibility();	
}
