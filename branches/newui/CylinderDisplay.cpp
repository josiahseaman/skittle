#include "CylinderDisplay.h"
#include "glwidget.h"
#include <sstream>

CylinderDisplay::CylinderDisplay(UiVariables* gui, GLWidget* gl)
{	
	glWidget = gl;
	ui = gui;
	string* seq = new string("AATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATTAATCGATCGTACGCTACGATCGCTACGCAGCTAGGACGGATT");
	sequence = seq;
	hidden = true;
	settingsTab = NULL;
	toggleButton = NULL;
	
	nucleotide_start = 1;
	scale = 1;
	changeWidth(ui->widthDial->value());
	changeSize(ui->sizeDial->value());
	upToDate = false;
	turnCylinder = 0;
	ntLinker = new NucleotideLinker();
	
	actionLabel = string("Alignment Cylinder");
	actionTooltip = string("Best 3D alignment as a Cylinder");
	actionData = actionLabel; 
	display_object = 0;
}	

CylinderDisplay::~CylinderDisplay(){
    glDeleteLists(display_object, 1);
}
//SEE note in AbstractGraph::createConnections
void CylinderDisplay::createConnections()
{
	connect( this, SIGNAL(widthChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(startChanged(int)), this, SIGNAL(displayChanged()));
	connect( this, SIGNAL(sizeChanged(int)), this, SIGNAL(displayChanged()));
	
	connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
	
	connect( ui->startDial, SIGNAL(valueChanged(int)), this, SLOT(changeStart(int)));
	connect( this, SIGNAL(startChanged(int)), ui->startDial, SLOT(setValue(int)));
	
	connect( ui->sizeDial, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect( this, SIGNAL(sizeChanged(int)), ui->sizeDial, SLOT(setValue(int)));
	
	connect( ui->scaleDial, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
	
}

void CylinderDisplay::createSquare()
{
		glBegin(GL_QUADS);
			glVertex3d(.5, .5, 0);
			glVertex3d(-.5, .5, 0);
			glVertex3d(-.5, -.5, 0);
			glVertex3d(.5, -.5, 0);
		glEnd();	
}

void CylinderDisplay::quickSquare()
{
   	glCallList(square);
}

void CylinderDisplay::display()
{
	checkVariables();
	if( !upToDate )
	{
    	//glDeleteLists(display_object, 1);
		//display_object = render();
	}
	glPushMatrix();
		glTranslated(width()/2, 0, 0);
		glRotated(turnCylinder, 0,1,0);//rotate cylinder around Y	
		//glCallList(display_object);
		render();
	glPopMatrix();
}

/* Cylinder display can have multiple widths in a single frame.
 * vector<point> width_list: x = nucleotide_start of the width frame
 * y = local_width between width_list[n] and width_list[n+1]
*/
GLuint CylinderDisplay::render()
{
	GLuint square = glGenLists(1);
	//GLuint list = glGenLists(1);
	
	glNewList(square, GL_COMPILE);
		createSquare();
	glEndList();

	//width_list = vector<float>(max_display_size, (float)Width);
	ntLinker->calculate(sequence->substr(nucleotide_start, display_size), Width);
	width_list = ntLinker->smooth(Width, 80);
		//ntLinker->tie_up_loose_ends(width_list);
		//ntLinker->cap_movement(width_list, 1);

    //glNewList(list, GL_COMPILE);
		
	if( !width_list.empty() )
	{
		max_width = 0;
		double pi = 3.141592653589793;
		double y = 0;
		double angle = 0;
		point p1 = point(0,0,0);
		float local_width = width_list[0];
		const char* genome = sequence->c_str() + nucleotide_start;
		glPushMatrix();
			glScaled(1,-1,1);
			for(int i = 0; i < display_size && y < 200; i++)
			{
				if(i >= width_list.size())
				{
					local_width = Width;
				}
				else
				{
					local_width = width_list[i];
				}
				
				if(local_width > max_width)
					max_width = local_width;
					
				y += 1.0 / local_width;
				angle += 1.0 / local_width * 360.0;

				color c1 = glWidget->colors( genome[i] );//TODO: Optimize pointer function call

				glPushMatrix();
					glRotated(angle, 0,1,0);//rotate cylinder around Y
					glTranslated(0, y, local_width/(pi*2));
				   	glColor3d(c1.r /255.0, c1.g /255.0, c1.b /255.0); 
				   	//paint_square(p1, c1);
				   	//createSquare();
					glCallList(square);
					//quickSquare();
				glPopMatrix();
			}
		glPopMatrix();
	}		

    //glEndList();
    upToDate = true;
    
    return 0;//(list);
}

int CylinderDisplay::width()
{
	double pi = 3.141592653589793;
	return (int)(max_width / pi);
}

string CylinderDisplay::mouseClick(point2D pt){	}

/******SLOTS*****/
void CylinderDisplay::saySomething()
{
	ui->print("I see you");
}
