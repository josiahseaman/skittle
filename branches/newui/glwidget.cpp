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
#include <QBitmap>
//#include <QCursor>

#include <math.h>
#include <sstream>
#include <vector>

#include <stdlib.h>
#include <ctime>

#include "glwidget.h"
#include "NucleotideDisplay.h"
#include "FrequencyMap.h"
#include "AnnotationDisplay.h"
#include "CylinderDisplay.h"
#include "AlignmentDisplay.h"
#include "OligomerDisplay.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "FastaReader.h"


using std::string;
//! [0]
GLWidget::GLWidget(UiVariables gui, QWidget* parentWidget)
    : QGLWidget(parentWidget)
    //,ui(gui)
{
    ui = gui;
	parent = parentWidget;
	setMouseTracking(true);
	setMinimumWidth(100);
	setMinimumHeight(100);
	frame = 0;
	
	setupColorTable();
	reader = new FastaReader(this, &ui);
	//this needs to be moved .... somewhere
	connect(reader, SIGNAL(newFileRead(const string*)), this, SLOT(displayString(const string*)));
	
    trackReader = new GtfReader(ui);
	
    nuc = new NucleotideDisplay(&ui, this);
    freq = new FrequencyMap(&ui, this);
    freq->link(nuc);
    cylinder = new CylinderDisplay(&ui, this);
   	align = new AlignmentDisplay(&ui, this);
   	olig = new OligomerDisplay(&ui, this);
	highlight = new HighlightDisplay(&ui, this);
   	
   	graphs.push_back(cylinder);
   	graphs.push_back(nuc);
   	graphs.push_back(olig);
   	graphs.push_back(align);
   	graphs.push_back(freq);
   	graphs.push_back(highlight);
   	
	border = 10;    
    xPosition = 0;
    slideHorizontal(0);
    changeZoom(100);
    canvasWidth = 1;
	canvasHeight = 1;
	setTool(SELECT_TOOL);
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
   	createConnections();
   	createCursors();
//   	createButtons();

	srand(time(0));
}

GLWidget::~GLWidget()
{
	delete reader;
	delete trackReader;
	
	for(int i = graphs.size() -1; i >= 0; --i)
	{
		delete graphs[i];
	}
    makeCurrent();//TODO: What does this do???
}

void GLWidget::createButtons()
{
   	for(int i = 0; i < graphs.size(); ++i)
   		emit addGraphMode( graphs[i] );
   	emit addDivider();
}

void GLWidget::createConnections()
{
   	for(int i = 0; i < graphs.size(); ++i)
   	{
   		//connect( graphs[i], SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
   		connect( this, SIGNAL(useTextures(bool)), graphs[i], SLOT(useTextures(bool)) );
   		connect( graphs[i], SIGNAL(hideSettings(QScrollArea*)), this, SIGNAL(hideSettings(QScrollArea*)));
   		connect( graphs[i], SIGNAL(showSettings(QScrollArea*)), this, SIGNAL(showSettings(QScrollArea*)));
   		//graphs[i]->createConnections();
	}
	
	connect(trackReader,SIGNAL(BookmarkAdded(track_entry,string)), this,SLOT(addTrackEntry(track_entry,string)));

	/****CONNECT LOCAL VARIABLES*******/ 
	connect(ui.zoomDial,  SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	connect(ui.scaleDial, SIGNAL(valueChanged(int)), this, SLOT(updateDisplaySize()));
	connect(ui.widthDial, SIGNAL(valueChanged(int)), this, SLOT(updateDisplaySize()));
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(160, 60);
}

QSize GLWidget::sizeHint() const
{
    return QSize(450, 300);
}

double GLWidget::getZoom()
{
	return 100.0 / zTransOffset;//inverse of: zTransOffset = 10 + 190 / percent;
}

void GLWidget::setTotalDisplayWidth()
{	
	//ui.print("SetWidth: ", ui.widthDial->value());
	int total_width = border;
	for(int i = 0; i < graphs.size(); ++i)
	{
		if(graphs[i]->hidden == false)
	   		total_width += graphs[i]->width() + border;
	}
	double z = getZoom();
	
	int val = (int)max(0.0, (double)(total_width)*z - canvasWidth ) ;
	emit totalWidthChanged(val); 
}

//***********SLOTS*******************
void GLWidget::changeZoom(int z)
{
	double percent = z / 100.0;
	zTransOffset = 200.0 / percent;
	updateDisplaySize();
	setTotalDisplayWidth();
}

const string* GLWidget::seq()
{
	return reader->seq();
}

void GLWidget::displayString(const string* seq)
{
	ui.print("New sequence received.  Size:", seq->size());

	for(int i = 0; i < graphs.size(); ++i)
	{
		graphs[i]->setSequence(seq);
		graphs[i]->invalidate();
	}
	//ui.startDial->setValue(2);//TODO: bit of a cludge to reset the start position
	ui.startDial->setValue(1);
	/*float multiplier = seq->size() / (float)ui.sizeDial->value();
	int newScale = max(1, (int)(ui.scaleDial->value() * multiplier) );
	ui.scaleDial->setValue( newScale );*/
	
	emit displaySizeChanged();
}

void GLWidget::on_moveButton_clicked()
{
	setTool(MOVE_TOOL);
}

void GLWidget::on_selectButton_clicked()
{
	setTool(SELECT_TOOL);
}

void GLWidget::on_findButton_clicked()
{
	setTool(FIND_TOOL);
	highlight->ensureVisible();
}

void GLWidget::on_resizeButton_clicked()
{
	setTool(RESIZE_TOOL);
}
	
void GLWidget::on_zoomButton_clicked()
{
	setTool(ZOOM_TOOL);
}
	
void GLWidget::setTool(int tool)
{
	int oldTool = currentTool;
	currentTool = tool;
	switch(tool)
	{
		case MOVE_TOOL:
			changeCursor(Qt::SizeAllCursor);
			break;
		case RESIZE_TOOL:
			changeCursor(Qt::SizeHorCursor);
			break;
		case FIND_TOOL:
		case SELECT_TOOL:
			changeCursor(Qt::CrossCursor);
			break;
		case ZOOM_TOOL:
			setCursor(zoomInCursor);
			break;
		default:
			currentTool = oldTool;
			ui.print("Error: Tool ID unrecognized: ",  tool);
	}
}

void GLWidget::slideHorizontal(int x)
{
	if(x != xPosition)
	{
		xPosition = x;
		emit xOffsetChange((int)(x));
		updateDisplay();
	}
}

void GLWidget::updateDisplay()
{
	setTotalDisplayWidth();
	//updateDisplaySize();
	//ui.print("UpdateGL");
	redraw();
}

void GLWidget::updateDisplaySize()
{
	//ui.print("Updating Size: ", ui.sizeDial->value());
	QSize dimensions = size();
	double pixelHeight = dimensions.height();
	int w = ui.widthDial->value();
	double zoom = 100.0 / ui.zoomDial->value();
	int display_lines = static_cast<int>(pixelHeight / 3.0 * zoom + 0.5);
	
	ui.sizeDial->setSingleStep(w * 10);
	if(ui.sizeDial->value() !=  w * display_lines )
	{
		ui.sizeDial->setValue( w * display_lines );
		emit displaySizeChanged();
		//updateDisplay();
	}
}

/*AnnotationDisplay* GLWidget::addAnnotationDisplay(QString fName)
{
	string fileName = fName.toStdString();
	//if( fileName.empty() )
	//{
	//	fileName = trackReader->outputFile();
	//}
	AnnotationDisplay* tempTrackDisplay = NULL;
	int fileIndex = -1;
	for ( int n = 0; n < graphs.size(); n++)
	{
		AnnotationDisplay* testPtr = dynamic_cast<AnnotationDisplay*>(graphs[n]);
		if ( testPtr != NULL && testPtr->getFileName().compare(fileName) == 0 )
		{
			tempTrackDisplay = testPtr;
			fileIndex = n;
			break;
		}
	}
	//if not found: exising track display
	if (fileIndex == -1)
	{
		vector<track_entry> track = trackReader->readFile(QString(fileName.c_str()));

		ui.print("Annotations Received: ", track.size());
		if( track.size() > 0)// || trackReader->outputFile().compare(fileName) == 0 )//
		{
			tempTrackDisplay = new AnnotationDisplay(&ui, this, fileName);
			graphs.insert(graphs.begin(), static_cast<AnnotationDisplay*>(tempTrackDisplay) );
			connect(tempTrackDisplay, SIGNAL(displayChanged()), this, SLOT(updateDisplay()));
			tempTrackDisplay->newTrack( track );
		}
	}
	return tempTrackDisplay;
}/**/

AnnotationDisplay* GLWidget::addAnnotationDisplay(QString fName)
{
	string fileName = fName.toStdString();
	if( fileName.empty() )
	{
		fileName = trackReader->outputFile();
	}

	AnnotationDisplay* tempTrackDisplay = findMatchingAnnotationDisplay(fileName);
	if( tempTrackDisplay != NULL)
	{
		ErrorBox msg("The file is already open");
	}
	else
	{
		vector<track_entry> track = trackReader->readFile(QString(fileName.c_str()));
		ui.print("Annotations Received: ", track.size());
		if( track.size() > 0)// || trackReader->outputFile().compare(fileName) == 0 )//
		{
			tempTrackDisplay = new AnnotationDisplay(&ui, this, fileName);
			graphs.insert(graphs.begin(), static_cast<AnnotationDisplay*>(tempTrackDisplay) );
			connect(tempTrackDisplay, SIGNAL(displayChanged()), this, SLOT(updateDisplay()));
			tempTrackDisplay->newTrack( track );
			tempTrackDisplay = NULL;
		}
	}
	return tempTrackDisplay;
} 

AnnotationDisplay* GLWidget::findMatchingAnnotationDisplay(string fileName)
{
	AnnotationDisplay* tempTrackDisplay = NULL;
	for ( int n = 0; n < graphs.size(); n++)
	{
		AnnotationDisplay* testPtr = dynamic_cast<AnnotationDisplay*>(graphs[n]);
		if ( testPtr != NULL && testPtr->getFileName().compare(fileName) == 0 )
		{
			tempTrackDisplay = testPtr;
			break;
		}	
	}
	return 	tempTrackDisplay;
}

void GLWidget::addTrackEntry(track_entry entry, string gtfFileName)
{
	AnnotationDisplay* trackDisplay = addAnnotationDisplay(QString(gtfFileName.c_str()));
	if (trackDisplay != NULL )
	{
		trackDisplay->addEntry(entry);
	}
}

void GLWidget::useHardwareAcceleration(bool b)
{
	emit useTextures(b);
}

/*****************FUNCTIONS*******************/

int GLWidget::tool()
{
	return currentTool;
}

//***********KEY HANDLING**************
void GLWidget::keyPressEvent( QKeyEvent *event )
{
	if( event->modifiers() & Qt::SHIFT && tool() == ZOOM_TOOL)
		setCursor(zoomOutCursor);
		
	int step = 10;	
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
	updateDisplay();
}

void GLWidget::keyReleaseEvent( QKeyEvent *event )
{
	if( event->key() == Qt::Key_Shift && tool() == ZOOM_TOOL)
	{
		setCursor(zoomInCursor);		
		event->accept();
	}
	else{
		QGLWidget::keyReleaseEvent( event );
	}
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
	for(int i = 0; i < graphs.size(); ++i)
		graphs[i]->setSequence(seq());
	
    qglClearColor(QColor::fromRgbF(0.5, 0.5, 0.5));
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
   	marker = 0;//glGenLists(1);

}

void GLWidget::paintGL()
{
	/** /
	if(!align->hidden)//TODO: move this inside AlignmentDisplay
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
				emit scaleChanged(scale);//ui.scaleDial->setValue(
				return;
			}
		}
	}/**/  
	//ui.print("Frame: ", ++frame);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
	    glTranslated(-xPosition, 0, 0);
	    double zoom = getZoom();
	    glScaled(zoom, zoom, zoom);
	    glTranslated(border,0,0);//to get zoom working right
	    if( tool() == SELECT_TOOL || tool() == FIND_TOOL)
	    {
			glCallList(marker);//possibly replace this with a blinking cursor
		}	
		for(int i = 0; i < (int)graphs.size(); ++i)
		{
			if(!graphs[i]->hidden)
			{
				graphs[i]->display();
				graphs[i]->displayLegend(canvasWidth / getZoom(), canvasHeight / getZoom());
                glTranslated(graphs[i]->width() + border, 0 , 0);
			}
		}
    glPopMatrix();
}

void GLWidget::resizeGL(int width, int height)
{
	
	glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	float pixelToGridRatio = 6.0;
	canvasWidth = width / pixelToGridRatio;
	canvasHeight = (height / pixelToGridRatio);
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
    updateDisplaySize();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
		placeMarker(event->pos());
	QPointF qp = pixelToGlCoords(event->pos());
	int x = (int)(qp.x());
	int y = (int)(qp.y());
		
	point2D oglCoords = point2D(x,y);	
	
	if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
	{
		vector<string> responses;
		/*Progressively decrement x based on the cumulative width of modules*/
		/*The order here is important and should match the left to right display order*/
		for(int i = 0; i < (int)graphs.size(); ++i)
		{
			if(!graphs[i]->hidden)
			{
				responses.push_back(graphs[i]->mouseClick(oglCoords));
				oglCoords.x -= graphs[i]->width() + border;
			}
		}
		if(tool() == FIND_TOOL)
		{
			for(int i = 0; i < (int)responses.size(); ++i)
			{
				if(!(responses[i].empty()))
				{
					QString copy(responses[i].c_str());
					highlight->setHighlightSequence(copy);
				}
			}
		}
	}
	if(tool() == ZOOM_TOOL)
	{
		float zoomFactor = 1.2;
		if(event->modifiers() & Qt::SHIFT) zoomFactor = .8;
			
		int scale = ui.scaleDial->value();//take current scale
		int index = oglCoords.y * (ui.widthDial->value()/scale) + oglCoords.x;
		index *= scale;
		index = max(0, index + ui.startDial->value());
		int newSize = (int)(ui.sizeDial->value() / zoomFactor);//calculate new projected size
		ui.startDial->setValue( index - (newSize/2) );//set start as centered point - size/2
		//size should recalculate
		int newScale = (int)(scale / zoomFactor) + (zoomFactor > 1.0? 0 : 1);//reduce scale by 10-20%  (Nx4)
		int zoom = ui.zoomDial->value();
		if( zoomFactor > 1.0 )  // we're zooming in
		{
			if(scale == 1)
				ui.zoomDial->setValue( zoom * zoomFactor );
			else
                emit scaleChangedSmart(newScale); //ui.scaleDial->setValue(newScale);//set scale to the new value
		}
		else//zooming out
		{
			if(zoom > 100)
				ui.zoomDial->setValue( zoom * zoomFactor );
			else
				ui.scaleDial->setValue(newScale);//set scale to the new value
		}
		this->updateDisplay();
	}
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPointF old = pixelToGlCoords(lastPos);
	QPointF evn = pixelToGlCoords(QPoint(event->x(), event->y()));
	if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
		placeMarker(event->pos());
    double zoom = getZoom();
    float dx = (evn.x() - old.x()) * zoom;
    float dy = (evn.y() - old.y()) * zoom;
    if (event->buttons() & Qt::LeftButton) 
	{
		if(event->modifiers() & Qt::ControlModifier)//Qt::ControlModifier
		{
			translateOffset(-dx, dy);
		}
		else
		{
			if(tool() == MOVE_TOOL)
				translate(-dx, dy);
			if(tool() == RESIZE_TOOL)
			{
				translate(0, dy);//still scroll up/down
				int value = static_cast<int>(dx * ui.scaleDial->value()*2.0 + ui.widthDial->value() + 0.5);
				ui.widthDial->setValue(value);
				
			}
			updateDisplay();
		}
    } 
    lastPos = event->pos();
}


void GLWidget::translate(float dx, float dy)
{
	if(dy != 0.0)
	{
		int sign = (int)(dy / fabs(dy));
		int move = -1* static_cast<int>(dy  + (sign*0.5)) * ui.widthDial->value() * 2;
		int current = ui.startDial->value();
		ui.startDial->setValue( max(1, current+move) );
	}
	emit xOffsetChange((int)(xPosition + dx + .5));
}

void GLWidget::translateOffset(float dx, float dy)
{
	int moveUp = 0;
	if(dy != 0.0)
	{
		int sign = (int)(dy / fabs(dy));
		moveUp = -1* static_cast<int>(dy  + (sign*0.5)) * ui.widthDial->value() * 2;	
	}
	int current = ui.offsetDial->value();
	if(dx != 0.0)
	{
		int sign = (int)(dx / fabs(dx));
		dx = (int)(dx + (0.5 * sign));
	}
	ui.offsetDial->setValue( (int)(current + moveUp + dx));
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
	//glDeleteLists(marker, 1);	
	marker = glGenLists(1);
    glNewList(marker, GL_COMPILE);
		nuc->paint_square(point(x, -y, 0), color(255,255,0));
	glEndList();

    updateDisplay();
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

void GLWidget::loadFile(QString fileName)
{
	if(parent != NULL)
		parent->setWindowTitle( reader->trimFilename(fileName.toStdString()).c_str());
	reader->readFile(fileName);
}

vector<QScrollArea*> GLWidget::settingsUi()
{
	vector<QScrollArea*> tabs;
	
	for(int i =0; i < (int)graphs.size(); ++i)
	{
		QScrollArea* tab = graphs[i]->settingsUi();
		if(tab != NULL)
			tabs.insert(tabs.begin(),tab);
	}
	return tabs;
}

void GLWidget::createCursors()
{
	QBitmap pic = QBitmap(":/zoomIn.png");
	if(pic.isNull())
	{
		ui.print("Warning: Null pic");
	}
	QBitmap mask = QBitmap(":/zoomIn-mask.png");
	if(mask.isNull())
	{
		ui.print("Warning: Null mask");
	}
	zoomInCursor = QCursor( pic, mask);  
	QBitmap pic2 = QBitmap(":/zoomOut.png");
	if(pic2.isNull())
	{
		ui.print("Warning: Null pic2");
	}
	zoomOutCursor = QCursor( pic2, mask);  
}

void GLWidget::reportOnFinish(){
	ui.print("The Editing has Finished");
}
/****************************************
 * Rethinking this strategy:
 ****************************************/
//~ void setUi(UiVariables master = NULL){
	//~ if (NULL == master) return;
	//~ ui = master
//~}
