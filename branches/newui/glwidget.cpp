/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>
#include <sstream>
#include <vector>

#include "glwidget.h"
#include "UiVariables.h"
//! [0]
GLWidget::GLWidget(UiVariables gui, QWidget *parent)
    : QGLWidget(parent)
{
	ui = gui;
	setMouseTracking(true);
	setMinimumWidth(80);
	setMinimumHeight(300);
	frame = 0;
	
	setupColorTable();
    nuc = new NucleotideDisplay(&ui, this);
    freq = new FrequencyMap(&ui, this);
    gtfTrack = new AnnotationDisplay(&ui, this);
    cylinder = new CylinderDisplay(&ui, this);
   	align = new AlignmentDisplay(&ui, this);
   	
   	//~ emit addGraphMode(gtfTrack);
   	emit addGraphMode(cylinder);
   	emit addGraphMode(nuc);
   	emit addGraphMode(freq);
   	emit addGraphMode(align);
   	
    marker = 0;

	border = 10;    
    xPosition = 0;
    slideHorizontal(0);
    changeZoom(100);
    canvasWidth = 1;
	canvasHeight = 1;
	setTool(RESIZE_TOOL);
	/*connect( ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(updateDisplaySize()));
    connect(ui.zoomDial, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));

	connect( nuc, SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
	connect( nuc, SIGNAL(sizeChanged(int)), this, SLOT(setPageSize()) );
	
    nuc->createConnections();
    freq->createConnections();
    cylinder->createConnections();
   	
	connect( ui.alignButton, SIGNAL(clicked()), this, SLOT(updateDisplay()));
	connect( ui.freqButton, SIGNAL(clicked()), this, SLOT(updateDisplay()));
	connect( ui.cylinderButton, SIGNAL(clicked()), this, SLOT(updateDisplay()));*/
	   
	setPageSize();
	
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
   	createConnections();
}

GLWidget::~GLWidget()
{
    makeCurrent();
}

void GLWidget::createConnections()
{
	connect(nuc, SIGNAL(displayChanged()), this, SLOT(updateDisplay()));
	connect( freq, SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
	connect( cylinder, SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
	connect( align, SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
	
	connect(nuc, SIGNAL(sizeChanged(int)), this, SLOT(setPageSize()) );
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(80, 60);
}

QSize GLWidget::sizeHint() const
{
    return QSize(80, 60);
}

double GLWidget::getZoom()
{
	return 100.0 / zTransOffset;//inverse of: zTransOffset = 10 + 190 / percent;
}

void GLWidget::setTotalDisplayWidth()
{	
	if( nuc && freq)
	{
		double z = getZoom();
		int total_width = border
			+ (!gtfTrack->hidden) * 6 
			+ (!nuc->hidden) * (nuc->width() + border) 
			+ (!freq->hidden) * (freq->width() + border)
			+ (!align->hidden) * (align->width() + border)
			+ (!cylinder->hidden) * ((int)(cylinder->maxWidth()*2) + border);
		//setMinimumWidth((double)(total_width)*z*6);
		//ui.horizontalScrollBar->setMaximum( (int)max(0.0, (double)(total_width)*z - canvasWidth ) );
	}
}
//***********SLOTS*******************
void GLWidget::changeZoom(int z)
{
	double percent = z / 100.0;
	zTransOffset = 200.0 / percent;
	updateDisplaySize();
	//setTotalDisplayWidth();
	//redraw();
}

void GLWidget::displayString(const string& seq)
{
	print("New sequence received.  Size:", seq.size());
	nuc->sequence= &seq;
	align->setSequence(&seq);
	freq->sequence = &seq;
	cylinder->sequence = &seq;
	
	setPageSize();
	ui.startDial->setValue(2);//TODO: bit of a cludge
	ui.startDial->setValue(1);
}

void GLWidget::on_moveButton_clicked()
{
	setTool(MOVE_TOOL);
}

void GLWidget::on_selectButton_clicked()
{
	setTool(SELECT_TOOL);
}

void GLWidget::on_resizeButton_clicked()
{
	setTool(RESIZE_TOOL);
}

void GLWidget::setPageSize()
{
	ui.verticalScrollBar->setMaximum( max(0, (int)(nuc->sequence->size() - ui.widthDial->value()) ) );//- ui.sizeDial->value()*.2
	ui.verticalScrollBar->setPageStep(nuc->max_display_size);
}
	
void GLWidget::setTool(int tool)
{
	switch(tool)
	{
		case MOVE_TOOL:
			currentTool = tool;
			changeCursor(Qt::SizeAllCursor);
			break;
		case RESIZE_TOOL:
			currentTool = tool;
			changeCursor(Qt::SizeHorCursor);
			break;
		case SELECT_TOOL:
			currentTool = tool;
			changeCursor(Qt::CrossCursor);
			break;
		default:
			print("Error: Tool ID unrecognized.", tool);
	}
}

void GLWidget::slideHorizontal(int x)
{
	if(x != xPosition)
	{
		xPosition = x;
		emit xOffsetChange((int)(x));
		redraw();
	}
}

void GLWidget::updateDisplay()
{
	setTotalDisplayWidth();
	//print("UpdateGL");
	redraw();
}

void GLWidget::updateDisplaySize()
{
	QSize dimensions = size();
	double pixelHeight = dimensions.height();
	int w = ui.widthDial->value();
	double zoom = 100.0 / ui.zoomDial->value();
	int display_lines = static_cast<int>(pixelHeight / 3.0 * zoom + 0.5);
	
	ui.sizeDial->setSingleStep(w * 10);
	ui.sizeDial->setValue( w * display_lines );
	if(ui.sizeDial->value() !=  w * display_lines )
		updateDisplay();
}

void GLWidget::newAnnotation(const vector<track_entry>& track)
{
	print("Annotations Received: ", track.size());
	gtfTrack->newTrack( &track );
}

/*****************FUNCTIONS*******************/

int GLWidget::tool()
{
	return currentTool;
}

//***********KEY HANDLING**************
void GLWidget::keyPressEvent( QKeyEvent *event )
{
	int step = 10;
	/*if( keyboardModifiers() == Qt::ControlModifier )
		step = 100;
	if( keyboardModifiers() == Qt::ShiftModifier )
		step = 1;
	*/
	int tenLines = ui.widthDial->value() * step;
    switch ( event->key() )//the keys should be passed directly to the widgets
    {
		case Qt::Key_Down:
			ui.startDial->setValue(ui.startDial->value() + tenLines);
			break;

		case Qt::Key_Up:
			ui.startDial->setValue(ui.startDial->value() - tenLines);
			break;

		case Qt::Key_Right:
			ui.widthDial->setValue(ui.widthDial->value() + ui.scaleDial->value());
			break;

		case Qt::Key_Left:
			ui.widthDial->setValue(ui.widthDial->value() - ui.scaleDial->value());
			break;

		default:
			event->ignore();
			return;
	}
	event->accept();
}


//***********Functions*************
QPointF GLWidget::pixelToGlCoords(QPoint pCoords, double z)
{
    glLoadIdentity();
	GLdouble position[3];
	GLdouble modelMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
	GLdouble projMatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	GLdouble mouseX = (GLdouble) pCoords.x();
	GLdouble mouseY = (GLdouble) pCoords.y();
	
	gluUnProject(
		mouseX,
		mouseY,
		z,
		modelMatrix,
		projMatrix,
		viewport,
		//the next 3 parameters are the pointers to the final array
		//coordinates. Notice that these MUST be double's
		&position[0], //-&gt; pointer to your own position (optional)
		&position[1], // id
		&position[2] // id
		);	
    double zoom = getZoom();	
	float x = (int)((mouseX / 6.0 + xPosition) / zoom ) - border ;
	float y = (int)((mouseY / 6.0) / zoom );
	
	return QPointF(x, y);
}

void GLWidget::initializeGL()
{
    qglClearColor(QColor::fromRgbF(0.5, 0.5, 0.5));
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
/*
void GLWidget::finalizeCalculations()
{
	gtfTrack->display();
	cylinder->display();
	nuc->display();
	int scale = ui.scaleDial->value();
	if(scale > 1){
		int displayWidth = ui.widthDial->value() / scale; 
		freq->calculate(nuc->nucleotide_colors, displayWidth);
	}
	freq->display();
	
}*/

void GLWidget::paintGL()
{
	if(!align->hidden)//TODO: move this inside AlignmentDisplay
	{
		int scale = ui.scaleDial->value();
		scale = max(4, (scale / 4) * 4);//enforces scale is a multiple of 4
		if(scale != ui.scaleDial->value())
		{
			ui.scaleDial->setValue(scale);
		}
	}
	//finalizeCalculations();

	//print("Frame: ", ++frame);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
	    glTranslated(-xPosition, 0, 0);
	    double zoom = getZoom();
	    glScaled(zoom, zoom, zoom);
	    glTranslated(border,0,0);//to get zoom working right
	    if( tool() == SELECT_TOOL)
			glCallList(marker);//possibly replace this with a blinking cursor
		if( !gtfTrack->hidden )
		{
			glTranslated(3, 0 , 0);
			gtfTrack->display();
			glTranslated(gtfTrack->width()+ border, 0 , 0);
		}
	    if(!cylinder->hidden)
		{
			int cylinderSize = (int)cylinder->maxWidth();
		    glTranslated(cylinderSize, 0 , 0);
			cylinder->display();
		    glTranslated(cylinderSize + border, 0 , 0);
		}
	    if(!nuc->hidden)
		{
			nuc->display();
		    glTranslated(nuc->width() + border, 0 , 0);
		}
		if(!align->hidden)
		{
			align->display();

			glPushMatrix();
				glTranslated(0, (-canvasHeight)/zoom, .1);
				glScaled(1,.5/zoom,1);
			    align->display_spectrum();
		    glPopMatrix();

	        glTranslated(align->width() + border, 0 , 0);
		}
		if(!freq->hidden)
		{
			freq->checkVariables();
			if((ui.scaleDial->value() > 1) && ( !freq->upToDate ))
			{
				if(!nuc->upToDate)
				{
					nuc->load_nucleotide();
				}
				int displayWidth = ui.widthDial->value() / ui.scaleDial->value(); 
				freq->calculate(nuc->nucleotide_colors, displayWidth);
			}
		    freq->display();
	        glTranslated(freq->width() + border, 0 , 0);
		}
    glPopMatrix();
}

void GLWidget::displayGraph(int graphMode)
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
	    glTranslated(-xPosition, 0, 0);
	    double zoom = getZoom();
	    glScaled(zoom, zoom, zoom);
	    glTranslated(border,0,0);//to get zoom working right
	    if( tool() == SELECT_TOOL)
			glCallList(marker);//TODO: possibly replace this with a blinking cursor
			
		//presets->display(graphMode);
		
    glPopMatrix();	
}

void GLWidget::resizeGL(int width, int height)
{
	
	glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	float pixelToGridRatio = 6.0;
	canvasWidth = width / pixelToGridRatio;
	canvasHeight = height / pixelToGridRatio;
	float left = 0;
	float right = left + width / pixelToGridRatio;
	float top = 0;
	float bottom = top - height / pixelToGridRatio;
	glOrtho(left, right, bottom, top, 0, 5000);
	
	gluLookAt(0, 0, 40, 					//position and direction
	      0, 0, 0,
		  0, 1, 0);
    
    glMatrixMode(GL_MODELVIEW);
    setTotalDisplayWidth();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	placeMarker(event->pos());
	QPointF qp = pixelToGlCoords(event->pos());
	int x = (int)(qp.x());
	int y = (int)(qp.y());
		
	point2D oglCoords = point2D(x,y);	
	
	if(tool() == SELECT_TOOL)
	{
		/*Progressively decrement x based on the cumulative width of modules*/
		/*The order here is important and should match the left to right display order*/
		if(!gtfTrack->hidden)
		{
			gtfTrack->mouseClick(oglCoords);
			oglCoords.x -= 6;
		}
		if(!cylinder->hidden)
		{
			oglCoords.x -= ((int)cylinder->maxWidth()) * 2 + border;	
		}
		if(!nuc->hidden)
		{
			nuc->mouseClick(oglCoords);
			oglCoords.x -= nuc->width() + border;
		}
		if(!align->hidden)
		{
			//align->mouseClick(oglCoords);
			oglCoords.x -= align->width() + border;
		}
		if(!freq->hidden)
		{
			freq->mouseClick(oglCoords);//pass click event
			oglCoords.x -= freq->width() + border;
		}
	}
	
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPointF old = pixelToGlCoords(lastPos);
	QPointF evn = pixelToGlCoords(QPoint(event->x(), event->y()));
	if(tool() == SELECT_TOOL)
		placeMarker(event->pos());
    float dx = evn.x() - old.x();
    float dy = evn.y() - old.y();
    if (event->buttons() & Qt::LeftButton) 
	{
	    double zoom = getZoom();
		if(tool() == MOVE_TOOL)
			translate(-dx * zoom, dy * zoom);
		if(tool() == RESIZE_TOOL)
		{
			translate(0, dy * zoom);//still scroll up/down
			int value = static_cast<int>(dx * zoom * ui.scaleDial->value()*2.0 + ui.widthDial->value() + 0.5);
			ui.widthDial->setValue(value);
		}
    } 
    lastPos = event->pos();
}


void GLWidget::translate(float dx, float dy)
{
	emit xOffsetChange((int)(xPosition + dx + .5));
	if(dy != 0.0)
	{
		int sign = (int)(dy / fabs(dy));
		int move = -1* static_cast<int>(dy  + (sign*0.5)) * ui.widthDial->value() * 2;
		int current = ui.startDial->value();
		ui.startDial->setValue( max(1, current+move) );
	}
	
    redraw();
}
 
void GLWidget::changeCursor(Qt::CursorShape cNumber)
{
	setCursor(QCursor(cNumber));
}

void GLWidget::placeMarker(QPoint pixelCoords)
{
	QPointF qp = pixelToGlCoords(pixelCoords);
	int x = (int)(qp.x());//round off
	int y = (int)(qp.y());//round off
    glDeleteLists(marker, 1);	
	marker = glGenLists(1);
    glNewList(marker, GL_COMPILE);
		nuc->paint_square(point(x+1, -y, 0), color(255,255,0));
	glEndList();

    redraw();
}

void GLWidget::redraw()
{
	updateGL();
}

/******COLOR MANAGEMENT**************/

color GLWidget::colors(char nucleotide)
{
	return colorTable[ (int)((unsigned char)nucleotide) ];
}

void GLWidget::setupColorTable()
{
	colorTable = vector<color>( 1024, color(150,150,150) );//default = dark grey
	
	{ 
		for(int i = 0; i <= 255; i++)
			colorTable[i] = spectrum( (double)i / 255.0);
	}
	//
	//based on: http://www.eyesopen.com/docs/html/vida/manual/Ch3DDisplaySubSecMoleculeProteins.html#SECTION0079212000000000000000
	//[1]  http://www.eyesopen.com/docs/html/vida/manual/node85.html#shapely
	colorTable[ (int)'A' ] = color(140, 255, 140);//Alanine
	//colorTable[ (int)'B' ] = color(255,255,255);//	UNUSED
	colorTable[ (int)'C' ] = color(255, 255, 112);//Cysteine
	colorTable[ (int)'D' ] = color(160, 0, 66);//Aspartic Acid
	colorTable[ (int)'E' ] = color(102, 0, 0);//GLutamic Acid
	colorTable[ (int)'F' ] = color(83, 76, 66);//Phenylalanine
	colorTable[ (int)'G' ] = color(255, 255, 255);//Glycine
	colorTable[ (int)'H' ] = color(112, 112, 255);//histidine
	colorTable[ (int)'I' ] = color(0, 76, 0);//Isoleucine
	//colorTable[ (int)'J' ] = color( 255,255,255);//	UNUSED
	colorTable[ (int)'K' ] = color(71, 71, 184);//lysine
	//colorTable[ (int)'L' ] = color(69, 94, 69);//	UNUSED
	colorTable[ (int)'M' ] = color(184, 160, 66);//Methionine
	colorTable[ (int)'N' ] = color(255, 124, 112);//Asparagine
	//colorTable[ (int)'O' ] = color( 255,255,255);//	UNUSED
	colorTable[ (int)'P' ] = color(82, 82, 82);//Proline
	colorTable[ (int)'Q' ] = color(255, 76, 76);//Glutamine GLN
	colorTable[ (int)'R' ] = color(0, 0, 124);//Arginine
	colorTable[ (int)'S' ] = color(255, 112, 66);//Serine
	colorTable[ (int)'T' ] = color(184, 76, 0);//Threonine
	//colorTable[ (int)'U' ] = color( 255,255,255);//	UNUSED
	colorTable[ (int)'V' ] = color(255, 140, 255);//Valine
	colorTable[ (int)'W' ] = color(79, 70, 0);//Tryptophan
	//colorTable[ (int)'X' ] = color( 255,255,255);//	UNUSED
	colorTable[ (int)'Y' ] = color(140, 112, 76);//Tyrosine TYR
	//colorTable[ (int)'Z' ] = color( 255,255,255);//	UNUSED
	
	
	//Override previous values
	colorTable[ (int)'A' ] = color(0, 0, 0);//BLACK - Adenine
	colorTable[ (int)'C' ] = color(255, 0, 0);//RED - Cytosine
	colorTable[ (int)'G' ] = color(0, 255, 0);//GREEN - Guanine
	colorTable[ (int)'T' ] = color(0, 0, 255);//BLUE - Thymine
	colorTable[ (int)'N' ] = color( 50, 50, 50);//not sequenced*/

}

color GLWidget::spectrum(double i)
{
	i = max(i, 0.0);
	i = min(i, 1.0);
	
	double r = 0.0;
	double g = 0.0;
	double b = 0.0;
	double fourth = 1/4.0;
		
	if(i <= 1*fourth)//falling blue, rising red - pink (purple)
	{
		r = (i-0*fourth) / fourth;
		b = 1.0 - (i-0*fourth) / fourth;
		i = 1.0;
	}
	/*else if(i <= 2*fourth)//All red, falling blue - red
	{
		r = 1.0;
		b = 1.0 - (i-1*fourth) / fourth;
		i = 1.0;
	}*/
	else if(i <= 2*fourth)//All Red, rising green - yellow
	{
		r = 1.0;
		g = (i-1*fourth) / fourth;
		i = 1.0;
	}
	else if(i <= 3*fourth)//All green, falling red - green
	{
		r = 1.0 - (i-2*fourth) / fourth;
		g = 1.0;
		i = 1.0;
	}
	else if(i <= 4*fourth)//All green, rising blue - cyan
	{
		g = 1.0;
		b = (i-3*fourth) / fourth;
		i = 1.0;
	}/*
	else if(i <= 5*fourth)//All blue, falling green - blue
	{
		g = 1.0 - (i-4*fourth) / fourth;
		b = 1.0;
		i = 1.0;
	}*/
	
//	shift from (-1 to 1) over to (0 to 1)
	int red =   static_cast<int>((r)* 255 + .5);
	int green = static_cast<int>((g)* 255 + .5);
	int blue =  static_cast<int>((b)* 255 + .5);

	return color(red, green, blue);
}

/**********Print Functions**********/
void GLWidget::print(const char* s)
{
	emit printText(QString(s));	
}

void GLWidget::print(std::string s)
{
	emit printText(QString(s.c_str()));	
}

void GLWidget::printHtml(std::string s)
{
	//QTextCursor cursor = textArea->textCursor();
	//cursor.movePosition(QTextCursor::End);
	emit printHtml(QString(s.c_str()));
}

void GLWidget::print(const char* s, int num)
{
	stringstream ss1;
	ss1 << s << num;

	emit printText(QString( ss1.str().c_str() ));	
}

void GLWidget::print(int num1, int num2)
{
	stringstream ss1;
	ss1 << num1 << ", " << num2;

	emit printText(QString( ss1.str().c_str() ));	
}

