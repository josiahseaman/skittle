#include "NucleotideDisplay.h"
#include "glwidget.h"
#include <sstream>

NucleotideDisplay::NucleotideDisplay(UiVariables* gui, GLWidget* gl)
{	
	glWidget = gl;
	ui = gui;
	string* seq = new string("AATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATT");	
	sequence = seq;
	
	textureBuffer = NULL;
	settingsTab = NULL;
	toggleButton = NULL;	
	hidden = false;
	
	nucleotide_start = 1;
	scale = 1;
	changeWidth(ui->widthDial->value());
	changeSize(ui->sizeDial->value());
	upToDate = false;
	
	invert = false;
	C = true;
	G = true;
	A = true;
	T = true;
	
	actionLabel = string("Nucleotide Display");
	actionTooltip = string("Nucleotides as Colors");
	actionData = actionLabel; 
	display_object = 0;
}

NucleotideDisplay::~NucleotideDisplay(){
    glDeleteLists(display_object, 1);
}
//see note in AbstractGraph::createConnections
void NucleotideDisplay::createConnections()
{
	connect( this, SIGNAL(widthChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(startChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(sizeChanged(int)), this, SIGNAL(displayChanged()));
	
	connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
	//connect( this, SIGNAL(widthChanged(int)), ui->widthDial, SLOT(setValue(int)));
	
	connect( ui->startDial, SIGNAL(valueChanged(int)), this, SLOT(changeStart(int)));
	connect( this, SIGNAL(startChanged(int)), ui->startDial, SLOT(setValue(int)));
	
	connect( ui->sizeDial, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect( this, SIGNAL(sizeChanged(int)), ui->sizeDial, SLOT(setValue(int)));

	connect( ui->scaleDial, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
}

void NucleotideDisplay::display()
{
	checkVariables();
	glPushMatrix();
	glScaled(1,-1,1);
		if(!upToDate)
			load_nucleotide();
		textureBuffer->display();
	glPopMatrix();
}

GLuint NucleotideDisplay::render()
{
	GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushMatrix();
	glScaled(1,-1,1);
		if(!upToDate)
			load_nucleotide();
		textureBuffer->display();
	glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void NucleotideDisplay::load_nucleotide()
{
	nucleotide_colors.clear();
	const char* genome = sequence->c_str() + nucleotide_start;
	if(scale > 1)
		color_compress();
	else
	{
		for(int i = 0; i < display_size; ++i)
			nucleotide_colors.push_back( glWidget->colors(genome[i]) );//TODO: Optimize pointer function call
	}
	
	loadTextureCanvas();
	upToDate = true;
}

void NucleotideDisplay::loadTextureCanvas()
{
	storeDisplay( nucleotide_colors, width() );
}	

void NucleotideDisplay::color_compress()
{
	nucleotide_colors.clear();
	
	int r = 0;
	int g = 0;
	int b = 0;
	int end = display_size + nucleotide_start - scale;
	const string& seq = *sequence;
	for(int i = nucleotide_start; i < end; )
	{
		for(int s = 0; s < scale; ++s)
		{
			color current = glWidget->colors(seq[i++]);
			r += current.r;
			g += current.g;
			b += current.b;
		}
		nucleotide_colors.push_back(color(r/scale, g/scale, b/scale));	
		r = 0;			
		g = 0;			
		b = 0;
	}
	upToDate = true;
}

/******SLOTS***** /
void NucleotideDisplay::changeWidth(int w)//Nucleotide Display changes Width internally to w/scale
{
	if(w < 1)
		w = 1;
	if(widthInBp() != w)
	{
		ui->print("Nucleotide Width");
		Width = max(1, w / ui->scaleDial->value() );
		emit widthChanged(w);
		invalidate();
	}	
}
/ **/
string NucleotideDisplay::mouseClick(point2D pt)
{
	//range check
	if( pt.x < width() && pt.x >= 0 && pt.y <= height() )
	{
		int index = pt.y * width() + pt.x;
		index *= scale;
		index = max(0, index);
		index = min((int)display_size-51, index);
		index = min( index + nucleotide_start, ((int)sequence->size()) - 51 );
//			ui->print("Index: ", index);
		string chromosome = glWidget->chromosomeName;
		stringstream ss;
		ss << "Index: " << index << "  Sequence: " << sequence->substr(index, 100);
		//ss<< "   <a href=\"http://genome.ucsc.edu/cgi-bin/hgTracks?hgsid=132202298&clade=mammal&org=Human&db=hg18&position="
		//<<chromosome<<":"<<index<<"-"<<index+200<<"&pix=800&Submit=submit\">View in Genome Browser</a> [external link]";
		//																											chr5			12389181	12390000
		ui->print(ss.str().c_str());
		return sequence->substr(index, 30);
	}
	else{
		return string();
	}
}
/**/

int NucleotideDisplay::widthInBp()//Nucleotide Display changes Width internally to w/scale
{
	return Width;
}
