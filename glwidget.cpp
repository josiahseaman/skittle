
#include <QtGui>
#include <QtOpenGL>
#include <QBitmap>
#if defined(Q_WS_MAC)
#include <OpenGL/glu.h>
#else
#ifndef QT_LINUXBASE
#   include <GL/glu.h>
#endif
#endif

#include <algorithm>
#include <math.h>
#include <sstream>
#include <vector>
#include <utility> //includes std::pair

#include <stdlib.h>
#include <ctime>

#include "glwidget.h"
#include "NucleotideDisplay.h"
#include "BiasDisplay.h"
#include "RepeatMap.h"
#include "AnnotationDisplay.h"
#include "CylinderDisplay.h"
#include "RepeatOverviewDisplay.h"
#include "OligomerDisplay.h"
#include "HighlightDisplay.h"
#include "GtfReader.h"
#include "FastaReader.h"
#include "SkittleUtil.h"

using std::string;
using std::pair;

/** ***********************************
GLWidget is the primary display interface for Skittle.  It handles the OpenGL
rendering and layout of all of the Graph display classes (everything that inherits
AbstractGraph).  It is the primary workhorse that instantiates all the file readers
and all the displays.  It also handles mouse tracking inside of OpenGL coordinate frame
since it controls the OpenGL viewframe settings (position, zoom, etc).  Skittle
uses a largely 2D isometric display surface.

GLWidget keeps track of what Graphs are currently displayed and arranges the position
of the Graphs on the screen accordingly.  It handles all of the mouse driven tools from
the toolbar.  For example, when the select tool is used and clicked at (302, 57) glwidget
figures out which display is within that x range (>302), adjusts the coordinate for margins
and then passes it to the Graph for processing.  The left-right displacement of the
horizontal scrollbar is handled by GLWidget.  Finally, key presses are sent to the active
GLWidget for interpretation.

There is one GLWidget per MdiChildWindow.  Each on connects to exactly one file.  New windows
with unique files can be created through the addViewAction which will create a new glWidget.
GLWidget inherits from QGLWidget which is the Qt interface layer.

Development: GLWidget has always threatened to be the god object of Skittle.  It has been
stripped down several times but it is still one of the largest class files.  More functionality,
such as mouse and keyboard interaction could be moved to new helper classes.
*/

GLWidget::GLWidget(UiVariables* gui, QWidget* parentWidget)
    : QGLWidget(parentWidget)
{
    ui = gui;
    parent = dynamic_cast<MdiChildWindow*>(parentWidget);
    setMouseTracking(true);
    setMinimumWidth(100);
    setMinimumHeight(100);
    frame = 0;


    setupColorTable();
    reader = new FastaReader(this, ui);
    trackReader = new GtfReader(ui);

    nuc = new NucleotideDisplay(ui, this);
    bias = new BiasDisplay(ui, this);
    freq = new RepeatMap(ui, this);
    freq->link(nuc);
    cylinder = new CylinderDisplay(ui, this);
    align = new RepeatOverviewDisplay(ui, this);
    olig = new OligomerDisplay(ui, this);
    highlight = new HighlightDisplay(ui, this);

    addGraph(olig);
    addGraph(align);
    addGraph(freq);
    addGraph(bias);
    addGraph(highlight);
    addGraph(nuc);
    addGraph(cylinder);

    selectionBoxVisible = false;
    border = 10;
    xPosition = 0;
    mouseMovePosition = QPoint();//you may want to initialize press and release points, start/end
    slideHorizontal(0);
    changeZoom(100);
    setTool(RESIZE_TOOL);
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
    makeCurrent();
}

void GLWidget::addGraph(AbstractGraph* graph)
{
    graphs.insert(graphs.begin(), graph );//these are added in reverse order for the sake of AnnotationDisplay

    //Note: The connection between displayChanged and updateDisplay is specifically used
    //for the case where the settingsUi tab causes an update that is only relevant to
    //one of the graphs.  This means that one graph is already invalidated and the others
    //do not need to change their data.
    connect( graph, SIGNAL(displayChanged()), this, SLOT(updateDisplay()) );
    connect( graph, SIGNAL(hideSettings(QScrollArea*)), this, SIGNAL(hideSettings(QScrollArea*)));
    connect( graph, SIGNAL(showSettings(QScrollArea*)), this, SIGNAL(showSettings(QScrollArea*)));

    emit addGraphMode(graph);
}

void GLWidget::createButtons()
{
    for(int i = (int)graphs.size()-1; i >= 0 ; --i)
        emit addGraphMode( graphs[i] );
    emit addDivider();
}

void GLWidget::createConnections()
{
    connect(trackReader,SIGNAL(BookmarkAdded(track_entry,string)), this,SLOT(addTrackEntry(track_entry,string)));

    /****CONNECT LOCAL VARIABLES*******/
    connect(ui->zoomDial,  SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
    connect(ui->widthDial, SIGNAL(valueChanged(int)), this, SLOT(updateDisplaySize()));
    connect(ui, SIGNAL(colorsChanged(int)), this, SLOT(invalidateDisplayGraphs()));
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
    //ui->print("SetWidth: ", ui->widthDial->value());
    int total_width = border;
    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        if(graphs[i]->hidden == false)
            total_width += graphs[i]->width() + border;
    }

    int val = (int)max(0.0, ((double)(total_width) - openGlGridWidth())*getZoom() ) ;
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

void GLWidget::displayString(const string* sequence)
{
    ui->print("New sequence received.  Size:", sequence->size());

    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        graphs[i]->setSequence(sequence);
        graphs[i]->invalidate();
    }
    ui->startDial->setValue(1);
    ui->changeScale(1);
    ui->changeWidth(128);

    //zoomExtents();

    updateDisplaySize();
    ui->zoomDial->setValue(ui->zoomDial->value() + 1);
    this->updateDisplay();
    ui->zoomDial->setValue(ui->zoomDial->value() - 1);
    this->updateDisplay();
}

void GLWidget::zoomExtents()
{
    zoomRange(1,seq()->size());
}
void GLWidget::zoomRange(int startIndex, int endIndex)
{
    float pixelWidth = (float)ui->widthDial->value() / (float)ui->scaleDial->value();
    float pixelsOnScreen = pixelWidth * (openGlGridHeight()-10);
    int selectionSize = abs(endIndex - startIndex);
    float requiredScale = (selectionSize) / pixelsOnScreen;
    int newScale = max(1, (int)(requiredScale + 0.5) );
    if (newScale == 1)
    {
        float requiredLines = selectionSize / pixelWidth;
        float screenHeight = openGlGridHeight() * ((float)ui->zoomDial->value() / 100); //how many pixels at zoom 100
        float screenWidth = openGlGridWidth() * ((float)ui->zoomDial->value() / 100) - 50;
        float requiredZoom = (screenHeight / requiredLines); // not percent based
        if (pixelWidth * (requiredZoom) > screenWidth) // if the zoom level makes the line wider than the screen just zoom to fit the widt
            requiredZoom = screenWidth / pixelWidth;
        int newZoom = max(100,(int)(requiredZoom * 100)); //now it's in percent
        ui->changeZoom(newZoom);
    }

    ui->startDial->setValue(min(startIndex,endIndex));
    ui->changeScale(newScale);
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

void GLWidget::on_addAnnotationButton_clicked()
{
    ui->print("ANNOTATE selected");
    setTool(ANNOTATE_TOOL);
}

void GLWidget::on_screenCaptureButton_clicked()
{
    makeCurrent();

    QImage image;

    int pictureWidth = getTotalPixelWidth();

    //Set the gl render width to the total graph's pixel widths
    int tempWidth = width();
    resize(pictureWidth, height());
    paintGL();

    if (format().rgba())
    {
        image = read_framebuffer(QSize(pictureWidth, height()), format().alpha(), false);
    }
    else
    {
#if defined(Q_WS_WIN) && !defined(QT_OPENGL_ES)
        image = QImage(pictureWidth, height(), QImage::Format_Indexed8);
        glReadPixels(0, 0, pictureWidth, height(), GL_COLOR_INDEX, GL_UNSIGNED_BYTE, image.bits());
        const QVector<QColor> pal = QColormap::instance().colormap();

        if(pal.size())
        {
            image.setNumColors(pal.size());
            for(int i = 0; i < pal.size(); i++)
            {
                image.setColor(i, pal.at(i).rgb());
            }
        }
        image = image.mirrored();
#endif
    }

    stringstream namestream;
    namestream << chromosomeName << "_w-" << ui->widthDial->value() << "_st-" << ui->startDial->value() << "_sc-" << ui->scaleDial->value() << ".png";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), namestream.str().c_str(), tr("Images (*.png *.jpg)"));

    image.save(filename);

    filename.prepend("Saved image: ");
    ui->print(filename.toStdString());

    //Set width back to what it should be
    this->resize(tempWidth, height());
    paintGL();
}

int GLWidget::getTotalPixelWidth()
{
    int pixelWidth = border;
    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        if(graphs[i]->hidden == false)
            pixelWidth += graphs[i]->width() + border;
    }
    return pixelWidth * getZoom() * 6;
}

QImage GLWidget::read_framebuffer(const QSize &size, bool alpha_format, bool include_alpha)
{
    QImage image(size, alpha_format ? QImage::Format_ARGB32 : QImage::Format_RGB32);

    int w = size.width();
    int h = size.height();

    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    convertFromGLImage(image, w, h, alpha_format, include_alpha);

    return image;
}

void GLWidget::convertFromGLImage(QImage &image, int w, int h, bool alpha_format, bool include_alpha)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        // OpenGL gives RGBA; Qt wants ARGB
        uint *p = (uint*)image.bits();
        uint *end = p + w*h;
        if (alpha_format && include_alpha) {
            while (p < end) {
                uint a = *p << 24;
                *p = (*p >> 8) | a;
                p++;
            }
        } else {
            // This is an old legacy fix for PowerPC based Macs, which
            // we shouldn't remove
            while (p < end) {
                *p = 0xff000000 | (*p>>8);
                ++p;
            }
        }
    } else {
        // OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
        for (int y = 0; y < h; y++) {
            uint *q = (uint*)image.scanLine(y);
            for (int x=0; x < w; ++x) {
                const uint pixel = *q;
                *q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff) | (pixel & 0xff00ff00);
                q++;
            }
        }

    }
    image = image.mirrored();
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
        changeCursor(Qt::OpenHandCursor);//previously Qt::SizeAllCursor
        break;
    case RESIZE_TOOL:
        changeCursor(Qt::SplitHCursor);//previously Qt::SizeHorCursor
        break;
    case FIND_TOOL:
        changeCursor(Qt::CrossCursor);
        break;
    case SELECT_TOOL:
        changeCursor(Qt::WhatsThisCursor);//previously Qt::CrossCursor);
        break;
    case ZOOM_TOOL:
        setCursor(zoomInCursor);
        break;
    case ANNOTATE_TOOL:
        changeCursor(Qt::IBeamCursor);
        break;
    default:
        currentTool = oldTool;
        ui->print("Error: Tool ID unrecognized: ",  tool);
    }
}

int GLWidget::tool()
{
    return currentTool;
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

void GLWidget::invalidateDisplayGraphs()
{
    setupColorTable();
    qDebug() << "GlWidget::invalidateDisplayGraphs " << ++frame;
    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        graphs[i]->invalidate();
    }
    updateDisplay();
}

void GLWidget::updateDisplay()
{
    setTotalDisplayWidth();
    //updateDisplaySize();
    redraw();
}

void GLWidget::updateDisplaySize()
{
    int w = ui->widthDial->value();

    ui->sizeDial->setSingleStep(w * 10);
    if(ui->sizeDial->value() !=  w * openGlGridHeight() )
    {
        ui->sizeDial->setValue( w * openGlGridHeight() );
        emit displaySizeChanged();
    }
}

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
        //if the display already exists, then it will simply return that one,
        //otherwise it will create a new Display
//        ErrorBox msg("The file is already open");
    }
    else
    {
        vector<track_entry> track = trackReader->readFile(QString(fileName.c_str()));
        ui->print("Annotations Received: ", track.size());
        if( track.size() > 0)// || trackReader->outputFile().compare(fileName) == 0 )//
        {
            tempTrackDisplay = new AnnotationDisplay(ui, this, fileName);
            addGraph(tempTrackDisplay);
            tempTrackDisplay->newTrack( track );
        }
    }
    return tempTrackDisplay;
} 

void GLWidget::jumpToAnnotation(bool forward)
{
    //scan for all the AnnotationDisplays
    vector<AnnotationDisplay*> annotations = getAllAnnotationDisplays();
    int startPosition = 1000000000;//end of file
    if (!forward)
        startPosition = 1;
    //have each submit the position of the next annotation
    for(int i = 0; i < (int)annotations.size(); ++i)
    {
        if(forward)
            startPosition = min(startPosition, annotations[i]->getNextAnnotationPosition());
        else //go backwards
            startPosition = max(startPosition, annotations[i]->getPrevAnnotationPosition());
    }
    //jump to the first one (min)
    if(startPosition < (int)seq()->size())
        ui->changeStart(startPosition);
    else if (startPosition <= 1)
        ui->print("You have reached the beginning of the file.");
    else
        ui->print("There are no annotations further in the file.");
}

AnnotationDisplay* GLWidget::findMatchingAnnotationDisplay(string fileName)
{
    vector<AnnotationDisplay*> aDisplays = getAllAnnotationDisplays();
    AnnotationDisplay* tempTrackDisplay = NULL;
    for ( int n = 0; n < (int)aDisplays.size(); n++)
    {
        if (aDisplays[n]  != NULL && aDisplays[n]->getFileName().compare(fileName) == 0 )
        {
            tempTrackDisplay = aDisplays[n];
            break;
        }
    }
    return 	tempTrackDisplay;
}

vector<AnnotationDisplay*> GLWidget::getAllAnnotationDisplays()
{
    vector<AnnotationDisplay*> annotations;
    for ( int n = 0; n < (int)graphs.size(); n++)
    {
        AnnotationDisplay* testPtr = dynamic_cast<AnnotationDisplay*>(graphs[n]);
        if ( testPtr != NULL )
            annotations.push_back(testPtr);
    }
    return 	annotations;
}

void GLWidget::addTrackEntry(track_entry entry, string gtfFileName)
{
    AnnotationDisplay* trackDisplay = addAnnotationDisplay(QString(gtfFileName.c_str()));
    if (trackDisplay != NULL )
    {
        trackDisplay->addEntry(entry);
    }
    makeCurrent();
}

/*****************FUNCTIONS*******************/


void GLWidget::zoomToolActivate(bool zoomOut)
{

    if(zoomOut || (abs(endPoint.y - startPoint.y) < 2 && abs(endPoint.x - startPoint.x) < 2))
    {
        float zoomFactor = 1.2;
        if(zoomOut)
            zoomFactor = 0.8;

        int scale = ui->scaleDial->value();//take current scale
        int index = startPoint.y * (ui->widthDial->value()/scale) + startPoint.x;
        index *= scale;
        index = max(0, index + ui->startDial->value());
        int newSize = (int)(ui->sizeDial->value() / zoomFactor);//calculate new projected size
        ui->startDial->setValue( index - (newSize/2) );//set start as centered point - size/2
        //size should recalculate
        int newScale = (int)(scale / zoomFactor) + (zoomFactor > 1.0? 0 : 1);//reduce scale by 10-20%  (Nx4)
        int zoom = ui->zoomDial->value();
        if( zoomFactor > 1.0 )  // we're zooming in
        {
            if(scale == 1)
                ui->changeZoom( zoom * zoomFactor );
            else
                ui->changeScale(newScale);
        }
        else //zooming out
        {
            if(zoom > 100)
                ui->changeZoom( max(100, ((int) (zoom * zoomFactor))) );
            else
                ui->changeScale(newScale);//set scale to the new value
        }
    }
    else // user selected range
    {
        pair<int,int> results = getSelectionOutcome();
        if(results.first != -1)
        {
            zoomRange(results.first, results.second);
        }
    }

    emit xOffsetChange(getSelectionOutcome(true).first);

}

pair<int, int> GLWidget::getSelectionOutcome(bool getGraphConstraints)
{
    int startIndex = 1;
    int endIndex = 1;
    int xOffset = 0;

    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        if(!graphs[i]->hidden)
        {
            pair<int,int> indices = graphs[i]->getIndicesFromPoints(point2D((startPoint.x - xOffset),startPoint.y), point2D((endPoint.x - xOffset),endPoint.y));
            startIndex = min(indices.first, indices.second);
            endIndex = max(indices.first, indices.second);
            if (startIndex > 0 && endIndex > 0 )//&& endIndex < seq()->size())
            {
                if(getGraphConstraints)
                {
                    return pair<int,int>(xOffset,(xOffset + graphs[i]->width()));
                }
                else // default behavior returns indicies
                    return pair<int,int>(startIndex,endIndex);
            }
            xOffset += graphs[i]->width() + border;
        }
    }
    return pair<int,int>(-1,-1);
}

//***********KEY HANDLING**************
void GLWidget::keyPressEvent( QKeyEvent *event )
{
    if( event->modifiers() & Qt::SHIFT && tool() == ZOOM_TOOL)
        setCursor(zoomOutCursor);

    int step = 10;
    int tenLines = ui->widthDial->value() * step;
    switch ( event->key() )//the keys should be passed directly to the widgets
    {
    case Qt::Key_Down:
        ui->changeStart(ui->startDial->value() + tenLines);
        break;

    case Qt::Key_Up:
        ui->changeStart(ui->startDial->value() - tenLines);
        break;

    case Qt::Key_Right:
        ui->changeWidth(ui->widthDial->value() + ui->scaleDial->value());
        break;

    case Qt::Key_Left:
        ui->changeWidth(ui->widthDial->value() - ui->scaleDial->value());
        break;

    default:
        event->ignore();
        return;
    }
    event->accept();
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
point2D GLWidget::pixelToGlCoords(QPoint pCoords, double z)
{
    double mouseX =  pCoords.x();
    double mouseY =  pCoords.y();
    //there was previously an unused GLunproject here
    double zoom = getZoom();
    int x = (int)((mouseX / 6.0 + xPosition) / zoom ) - border ;
    int y = (int)((mouseY / 6.0) / zoom );

    return point2D(x, y);
}

int GLWidget::openGlGridHeight()
{
    QSize dimensions = size();
    double pixelHeight = dimensions.height();
    double zoom = 100.0 / ui->zoomDial->value();
    float pixelToGridRatio = 3.0;
    int display_lines = static_cast<int>(pixelHeight / pixelToGridRatio * zoom + 0.5);

    return display_lines;
}

int GLWidget::openGlGridWidth()
{
    QSize dimensions = size();
    double pixelWidth = dimensions.width();
    double zoom = 100.0 / ui->zoomDial->value();
    float pixelToGridRatio = 3.0;
    int openGLWidth = static_cast<int>(pixelWidth / pixelToGridRatio * zoom + 0.5);

    return openGLWidth;
}

void GLWidget::initializeGL()
{
    qglClearColor(QColor::fromRgbF(0.5, 0.5, 0.5));//50% grey
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    marker = 0;//glGenLists(1);

    for(int i = 0; i < (int)graphs.size(); ++i)
        graphs[i]->setSequence(seq());
}

void GLWidget::paintGL()
{
    //    qDebug() << "GlWidget Frame: " << ++frame;
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
    if (selectionBoxVisible)// tool() == ZOOM_TOOL &&  )
    {
        drawSelectionBox(startPoint, endPoint);
    }
    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        if(!graphs[i]->hidden)
        {
            graphs[i]->display();
            graphs[i]->displayLegend(openGlGridWidth(), openGlGridHeight());
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

void pushNonEmpty(vector<string>& arr, string str)
{
    if(!str.empty())
        arr.push_back(str);
}

QString grabFirstNonEmpty(vector<string> array)
{
    for(int i = 0; i < (int)array.size(); ++i)
        if(!array[i].empty())
            return QString(array[i].substr(0,1000).c_str());//truncating the string in the case where NucleotideDisplay dumps a 1Mbp sequence

    return QString();
}

bool GLWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        point2D oglCoords = pixelToGlCoords(helpEvent->pos());
        QString text = grabFirstNonEmpty(mouseOverText(oglCoords));
        if( !text.isEmpty() )
        {
            QToolTip::showText(helpEvent->globalPos(), text);
        }
        else{
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }
    return QGLWidget::event(event);
}

vector<string> GLWidget::mouseOverText(point2D oglCoords)
{
    vector<string> responses;
    /*Progressively decrement x based on the cumulative width of modules
      The order here is important and should match the left to right display order*/
    for(int i = 0; i < (int)graphs.size(); ++i)
    {
        if(!graphs[i]->hidden)
        {
            if(tool() == SELECT_TOOL )
                pushNonEmpty(responses, graphs[i]->SELECT_MouseClick(oglCoords));
            if(tool() == FIND_TOOL)
                pushNonEmpty(responses, graphs[i]->FIND_MouseClick(oglCoords));
            oglCoords.x -= graphs[i]->width() + border;
        }
    }
    return responses;
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    parent->mousePressEvent(event);

    startPoint = pixelToGlCoords(event->pos());

    if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
        placeMarker(event->pos());

    if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
    {
        vector<string> responses = mouseOverText( startPoint);

        if(tool() == SELECT_TOOL)
        {
            for(int i = 0; i < (int)responses.size(); ++i)
                ui->print(responses[i]);
        }
        if(tool() == FIND_TOOL)
        {
            for(int i = 0; i < (int)responses.size(); ++i)
                highlight->setHighlightSequence(QString(responses[i].c_str()));
        }
    }
    if(tool() == MOVE_TOOL )
        changeCursor(Qt::ClosedHandCursor);
    if(tool() == ZOOM_TOOL || tool() == ANNOTATE_TOOL )
    {
        selectionBoxVisible = true;
    }

    endPoint = startPoint;

    mousePressPosition = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    point2D old = pixelToGlCoords(mouseMovePosition);
    endPoint = pixelToGlCoords(event->pos());

    if(tool() == SELECT_TOOL || tool() == FIND_TOOL)
        placeMarker(event->pos());
    double zoom = getZoom();
    float dx = (endPoint.x - old.x) * zoom;
    float dy = (endPoint.y - old.y) * zoom;

    if (event->buttons() & Qt::LeftButton)
    {
        if((tool()== RESIZE_TOOL || tool()== MOVE_TOOL) && (event->modifiers() & Qt::ControlModifier))
        {
            translateOffset(-dx, dy);
        }
        else{
            if(tool() == MOVE_TOOL)
                translate(-dx, dy);
            if(tool() == RESIZE_TOOL)
            {
                translate(0, dy);//still scroll up/down
                int value = static_cast<int>(dx * ui->scaleDial->value()*2.0 + ui->widthDial->value() + 0.5);
                ui->changeWidth(value);
            }
        }
        if(tool() == ZOOM_TOOL && selectionBoxVisible )
        {

        }

        invalidateDisplayGraphs();
    }
    mouseMovePosition = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseReleasePosition = event->pos();
    endPoint = pixelToGlCoords(event->pos());

    if(tool() == MOVE_TOOL )
        changeCursor(Qt::OpenHandCursor);
    if( selectionBoxVisible)
    {
        selectionBoxVisible = false;
        if(tool() == ZOOM_TOOL  )
        {
            bool zoomingOut = (event->modifiers() & Qt::SHIFT) || (event->button() == Qt::RightButton);
            if(zoomingOut)
                setCursor(zoomOutCursor);
            else
                setCursor(zoomInCursor);
            zoomToolActivate(zoomingOut);
        }
        if (tool() == ANNOTATE_TOOL )
        {
            pair<int,int> results = getSelectionOutcome();
            if(results.first != -1)
                trackReader->addBookmark(results.first, results.second);
        }
    }
}

//m:draw Selection box
void GLWidget::drawSelectionBox(point2D start,point2D end) //, int lineStart, int lineEnd
{
    if(selectionBoxVisible)
    {

        if (start.y > end.y || (start.y == end.y && start.x > end.x)) //if you drag up,
        {
            point2D temp = start;//swap the top and bottom of the box
            start = end;
            end = temp;
        }

        pair<int,int> graphConstraints = getSelectionOutcome(true); //
        int lineStart = graphConstraints.first;
        int lineEnd = graphConstraints.second;

        if (lineStart != -1) // force points to be in bounds and make pretty squared off boxes
        {
            if (start.x < lineStart)
                start.x = lineStart;
            if (start.x >= lineEnd)
            {
                start.x = lineStart;
                ++start.y;
            }
            if (end.x > lineEnd)
                end.x = lineEnd;
            if (end.x < lineStart)
            {
                end.x = lineEnd;
                --end.y;
            }


            // hairline edge
            color c = color(200,185,60);
            double lineThickness = 0.3;
            //draw the ragged box edge
            nuc->paint_line(point((lineStart - lineThickness), -(start.y + 1 - lineThickness),0),   point(start.x, -(start.y + 1),0), c); //top left
            nuc->paint_line(point((start.x - lineThickness), -(start.y - lineThickness),0),         point(start.x, -(start.y + 1),0), c); //top jog
            nuc->paint_line(point((start.x - lineThickness), -(start.y - lineThickness),0),         point((lineEnd + lineThickness), -start.y,0), c); //top right
            nuc->paint_line(point((lineStart - lineThickness), -(end.y + 1),0),                     point((end.x + lineThickness), -(end.y + 1 + lineThickness),0), c); //bottom left
            nuc->paint_line(point(end.x, -end.y,0),                                                 point((end.x + lineThickness), -(end.y + 1 + lineThickness),0), c); //bottom jog
            nuc->paint_line(point(end.x, -end.y,0),                                                 point((lineEnd + lineThickness), -(end.y + lineThickness),0), c); //bottom right
            nuc->paint_line(point((lineStart - lineThickness), -(start.y + 1 - lineThickness),0),   point(lineStart, -(end.y + 1 + lineThickness),0), c); //left
            nuc->paint_line(point(lineEnd, -start.y,0),                                             point((lineEnd + lineThickness), -(end.y + lineThickness),0), c); //right

            // adjust to taste
            c = color(255,235,80);
            lineThickness = max(1.0, (double)(2.75 - (ui->zoomDial->value())/400)); // thickness scales inversely to zoom level
            //        double lineThickness = 1.5;
            //draw the ragged box
            nuc->paint_line(point((lineStart - lineThickness), -(start.y + 1 - lineThickness),0),   point(start.x, -(start.y + 1),0), c); //top left
            nuc->paint_line(point((start.x - lineThickness), -(start.y - lineThickness),0),         point((lineEnd + lineThickness), -start.y,0), c); //top right
            nuc->paint_line(point((lineStart - lineThickness), -(end.y + 1),0),                     point((end.x + lineThickness), -(end.y + 1 + lineThickness),0), c); //bottom left
            nuc->paint_line(point(end.x, -end.y,0),                                                 point((lineEnd + lineThickness), -(end.y + lineThickness),0), c); //bottom right
            nuc->paint_line(point((lineStart - lineThickness), -(start.y + 1 - lineThickness),0),   point(lineStart, -(end.y + 1 + lineThickness),0), c); //left
            nuc->paint_line(point(lineEnd, -start.y,0),                                             point((lineEnd + lineThickness), -(end.y + lineThickness),0), c); //right

        }
    }
}

void GLWidget::translate(float dx, float dy)
{
    if(dy != 0.0)
    {
        int sign = (int)(dy / fabs(dy));
        int move = -1* static_cast<int>(dy  + (sign*0.5)) * ui->widthDial->value() * 2;
        int current = ui->startDial->value();
        ui->changeStart( max(1, current+move) );
    }
    emit xOffsetChange((int)(xPosition + dx + .5));
}

void GLWidget::translateOffset(float dx, float dy)
{
    int moveUp = 0;
    if(dy != 0.0)
    {
        int sign = (int)(dy / fabs(dy));
        moveUp = -1* static_cast<int>(dy  + (sign*0.5)) * ui->widthDial->value() * 2;
    }
    int current = ui->offsetDial->value();
    if(dx != 0.0)
    {
        int sign = (int)(dx / fabs(dx));
        dx = (int)(dx + (0.5 * sign));
    }
    ui->changeOffset( (int)(current + moveUp + dx));
}

void GLWidget::changeCursor(Qt::CursorShape cNumber)
{
    setCursor(QCursor(cNumber));
}

void GLWidget::placeMarker(QPoint pixelCoords)
{
    point2D pt = pixelToGlCoords(pixelCoords);
    //glDeleteLists(marker, 1);
    marker = glGenLists(1);
    glNewList(marker, GL_COMPILE);
    nuc->paint_square(point(pt.x, -pt.y, 0), color(255,255,0));
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



    int colorSetting = ui->getColorSetting();
    if(colorSetting == UiVariables::COLORBLINDSAFE)
    {
        colorTable[ (int)'A' ] = color(255, 102, 0);//Adenine
        colorTable[ (int)'C' ] = color(153, 0, 0);//Cytosine
        colorTable[ (int)'G' ] = color(51, 255, 51);//Guanine
        colorTable[ (int)'T' ] = color(0, 153, 204);//Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced

    }
    else if (colorSetting == UiVariables::BETTERCBSAFE)
    {
        colorTable[ (int)'A' ] = color(0, 204, 204);//Adenine
        colorTable[ (int)'C' ] = color(153, 255, 0);//Cytosine
        colorTable[ (int)'G' ] = color(204, 0, 102);//Guanine
        colorTable[ (int)'T' ] = color(255, 102, 0);//Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }
    else if (colorSetting == UiVariables::DARK)
    {
        colorTable[ (int)'A' ] = color(131, 106, 7);//Adenine
        colorTable[ (int)'C' ] = color(110, 54, 76);//Cytosine
        colorTable[ (int)'G' ] = color(13, 94, 58);//Guanine
        colorTable[ (int)'T' ] = color(40, 75, 163);//Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }
    else if (colorSetting == UiVariables::DRUMS)
    {
        colorTable[ (int)'A' ] = color(80, 80, 255);//Adenine
        colorTable[ (int)'C' ] = color(224, 0, 0);//Cytosine
        colorTable[ (int)'G' ] = color(0, 192, 0);//Guanine
        colorTable[ (int)'T' ] = color(230, 230, 0);//Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }
    else if (colorSetting == UiVariables::BLUES)
    {
        colorTable[ (int)'A' ] = color(141, 0, 74);//Adenine
        colorTable[ (int)'C' ] = color(82, 0, 124);//Cytosine
        colorTable[ (int)'G' ] = color(17, 69, 134);//Guanine
        colorTable[ (int)'T' ] = color(14, 112, 118);//Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }
    else if (colorSetting == UiVariables::REDS)
    {
        colorTable[ (int)'A' ] = color(141, 0, 74);// Adenine
        colorTable[ (int)'C' ] = color(159, 0, 0);// Cytosine
        colorTable[ (int)'G' ] = color(196, 90, 6);// Guanine
        colorTable[ (int)'T' ] = color(218, 186, 8);// Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }

    // might be interesting for "Highlight A", "Highlight C", etc options which would be rgb except for the one Nuc that was white
    else // default values AKA "Classic"
    {
        colorTable[ (int)'A' ] = color(0, 0, 0);//BLACK - Adenine
        colorTable[ (int)'C' ] = color(255, 0, 0);//RED - Cytosine
        colorTable[ (int)'G' ] = color(0, 255, 0);//GREEN - Guanine
        colorTable[ (int)'T' ] = color(0, 0, 255);//BLUE - Thymine
        colorTable[ (int)'N' ] = color( 200, 200, 200);//not sequenced
    }
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
        parent->setWindowTitle( trimPathFromFilename(fileName.toStdString()).c_str());

    removeAllAnnotations();
    reader->readFile(fileName);
}

bool GLWidget::removeAllAnnotations()
{
    vector<AnnotationDisplay*> list = getAllAnnotationDisplays();
    for(int i = list.size()-1; i >= 0; --i)
    {
        vector<AbstractGraph*>::iterator bob = std::find(graphs.begin(), graphs.end(),
                                                        static_cast<AbstractGraph*>(list[i]));
        graphs.erase(bob);
        delete list[i];//apparently .erase() calls the destructor, but what if it's a pointer?
    }
    return true;
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
        ui->print("Warning: Null pic");
    }
    QBitmap mask = QBitmap(":/zoomIn-mask.png");
    if(mask.isNull())
    {
        ui->print("Warning: Null mask");
    }
    zoomInCursor = QCursor( pic, mask);
    QBitmap pic2 = QBitmap(":/zoomOut.png");
    if(pic2.isNull())
    {
        ui->print("Warning: Null pic2");
    }
    zoomOutCursor = QCursor( pic2, mask);
}

void GLWidget::reportOnFinish(int i){
    ui->print("The Editing has Finished");
    ui->printNum(i);
}
/****************************************
 * Rethinking this strategy:
 ****************************************/
//~ void setUi(UiVariables master = NULL){
//~ if (NULL == master) return;
//~ ui = master
//~}
