#include "CylinderDisplay.h"
#include "glwidget.h"
#include <sstream>

/** *******************
  CylinderDisplay is a 3D Graph class that is designed to bring all tandem repeats
  on the screen into alignment simultaneously.  It does this by dynamically changing
  the circumference of each line of the cylinder based on the sequence content.  This
  means that it has to decide the optimum width for every few nucleotides.  This is fairly
  processor intensive.  Adding 3D rendering on top of that makes this a complicated class.

  The algorithm for calculating continuous alignment has been offloaded to the NucleotideLinker
  class, while the logic for 3D rotation and display exists in CylinderDisplay.

  Development: Historically, CylinderDisplay was designed to model a hypothesis for DNA folding inside of
  the nucleus.  Thus the 3D display.  If the purpose is simply to show all repeats in alignment,
  then this Graph should be succeeded by a 2D Ragged Edge Graph that uses similar algorithms.
  2D would be better in that the user could see all of the sequence and simpler to render.

*********************/

CylinderDisplay::CylinderDisplay(UiVariables* gui, GLWidget* gl)
    :AbstractGraph(gui, gl)
{
    turnCylinder = 0;
    ntLinker = new NucleotideLinker();

    actionLabel = string("Alignment Cylinder");
    actionTooltip = string("Best 3D alignment as a Cylinder");
    actionData = actionLabel;
}	

CylinderDisplay::~CylinderDisplay(){
    glDeleteLists(display_object, 1);
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

void CylinderDisplay::calculateOutputPixels()
{
    glDeleteLists(display_object, 1);
    display_object = render();
}

void CylinderDisplay::display()
{
    checkVariables();
    if( !upToDate )
    {
        calculateOutputPixels();
    }
    glPushMatrix();
    glTranslated(width()/2, 0, 0);
    glRotated(turnCylinder, 0,1,0);//rotate cylinder around Y
    glCallList(display_object);
    glPopMatrix();
}

/* Cylinder display can have multiple widths in a single frame.
 * vector<point> width_list: x = nucleotide_start of the width frame
 * y = local_width between width_list[n] and width_list[n+1]
*/
GLuint CylinderDisplay::render()
{
    qDebug() << "CylinderDisplay::render(): " << ++frameCount;
    GLuint square = glGenLists(1);
    GLuint cylinder_Display_list = glGenLists(1);

    glNewList(square, GL_COMPILE);
    createSquare();
    glEndList();

    int min_width = min(150, max(1, ui->getWidth() / 3 ));
    ntLinker->calculate(sequence->substr(ui->getStart(glWidget), current_display_size()), min_width);
    width_list = ntLinker->smooth(min_width, 80);
    //ntLinker->tie_up_loose_ends(width_list);
    //ntLinker->cap_movement(width_list, 1);

    glNewList(cylinder_Display_list, GL_COMPILE);

    if( !width_list.empty() )
    {
        max_width = 0;
        double pi = 3.141592653589793;
        double y = 0;
        double angle = 0;
        int min_width = min(150, max(1, ui->getWidth() / 3 ));
        float local_width = width_list[0];
        const char* genome = sequence->c_str() + ui->getStart(glWidget);//TODO:not a particularly safe way of accessing
        glPushMatrix();
        glScaled(1,-1,1);
        int temp_display_size = current_display_size();
        for(int i = 0; i < temp_display_size && y < 200; i++)
        {
            if(i >= (int)width_list.size())
            {
                local_width = min_width;
            }
            else
            {
                local_width = width_list[i];
            }

            if(local_width > max_width)
                max_width = local_width;

            y += 1.0 / local_width;
            angle += 1.0 / local_width * 360.0;

            color c1 = glWidget->colors( genome[i] );

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

    glEndList();
    upToDate = true;
    
    return cylinder_Display_list;
}

int CylinderDisplay::width()
{
    double pi = 3.141592653589793;
    return (int)(max_width / pi);
}

string CylinderDisplay::SELECT_MouseClick(point2D pt)
{
    return string("");
}

/******SLOTS*****/
