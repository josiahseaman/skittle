#include <QtGui/QTextEdit>
#include <QtGui/QSpinBox>
#include <QString>
#include <sstream>
#include <algorithm>
#include "UiVariables.h"

using std::max;
/** *******************************
This class is a container class for the set of 5 global dials that affect all
Graph visualizations.  Those dials are: start, width, size, scale, and zoom.
It also contains a pointer to the local offsetDial for multiple windows.
Finally, UiVariables has a pointer to the textArea of "Information Display"
and convenience functions for printing out information to the text area.  This
is useful for writing debugging notes since cout is generally unavailable.
You should also use the print functions to provide data to the user in a format
that can be copy and pasted without the need to write a file.
**********************************/

UiVariables::UiVariables(QTextEdit* text)
{
    textArea = text;
    if(textArea != NULL)
        textArea->toPlainText();

    widthDial = new QSpinBox();
    widthDial->setMinimum(1);
    widthDial->setMaximum(1000000000);
    widthDial->setValue(128);
    widthDial->setSuffix(" bp");
    widthDial->setButtonSymbols(QAbstractSpinBox::NoButtons);

    scaleDial = new QSpinBox();
    scaleDial->setMinimum(1);
    scaleDial->setMaximum(100000);
    scaleDial->setValue(1);
    scaleDial->setSingleStep(4);
    scaleDial->setSuffix(" bp/pixel");
    scaleDial->setButtonSymbols(QAbstractSpinBox::NoButtons);

    zoomDial = new QSpinBox();
    zoomDial->setMinimum(1);
    zoomDial->setMaximum(100000);
    zoomDial->setSingleStep(10);
    zoomDial->setValue(100);
    zoomDial->setButtonSymbols(QAbstractSpinBox::NoButtons);

    startDial = new QSpinBox();
    startDial->setMinimum(1);
    startDial->setMaximum(400000000);
    startDial->setValue(1);
    startDial->setButtonSymbols(QAbstractSpinBox::NoButtons);

    sizeDial = new QSpinBox();
    sizeDial->setMinimum(1000);
    sizeDial->setMaximum(400000000);//something very large MAX_INT?
    sizeDial->setSingleStep(1000);
    sizeDial->setValue(10000);
    sizeDial->setSuffix(" bp");
    sizeDial->setButtonSymbols(QAbstractSpinBox::NoButtons);

    offsetDial = new QSpinBox();
    offsetDial->setMinimum(-40000000);
    offsetDial->setMaximum(40000000);
    offsetDial->setValue(0);
    offsetDial->setSingleStep(widthDial->value());
    offsetDial->hide();
    oldScale = 1;
    oldWidth = 128;

    colorSetting = string("Classic");

    //These lines make the Graphs respond immediately to typing rather than waiting for the user to finish
    /*connect(widthDial, SIGNAL(valueChanged(int)), this, SIGNAL(internalsUpdated()));
    connect(scaleDial, SIGNAL(valueChanged(int)), this, SIGNAL(internalsUpdated()));
    connect(zoomDial, SIGNAL(valueChanged(int)), this, SIGNAL(internalsUpdated()));
    connect(startDial, SIGNAL(valueChanged(int)), this, SIGNAL(internalsUpdated()));*/
}

UiVariables::~UiVariables()
{
    //delete all the individual dial pointers?
}

void UiVariables::print(char const * s)
{
	if(textArea != NULL)
	{
		QString name(s);
		textArea->append(name);	
	}
}

void UiVariables::print(std::string s)
{
	textArea->append(QString(s.c_str()));	
}

void UiVariables::printHtml(std::string s)
{
	if(textArea != NULL)
		textArea->insertHtml(QString(s.c_str()));
}

void UiVariables::print(const char* s, int num)
{
	std::stringstream ss1;
	ss1 << s << num;

	textArea->append(QString( ss1.str().c_str() ));	
}

void UiVariables::printNum(int num)
{
	std::stringstream ss1;
	ss1 << num;

	textArea->append(QString( ss1.str().c_str() ));	
}
/*
void UiVariables::print(int num1, int num2)
{
	stringstream ss1;
	ss1 << num1 << ", " << num2;

	textArea->append(QString( ss1.str().c_str() ));	
}*/

void UiVariables::changeWidth(int newWidth)
{
    if(newWidth < 1)
        newWidth = 1;

    if (newWidth != oldWidth)
    {
        int newScale = oldScale;
        int displayWidth = newWidth / oldScale;
        if ( displayWidth < 1 || displayWidth > maxSaneWidth)
        {
            newScale = int(max(double(1.0), double(oldScale) * ( double(newWidth) / double(oldWidth))));
            scaleDial->setValue(newScale);
        }
        widthDial->setValue(newWidth);
        oldWidth = newWidth;
        oldScale = newScale;
        emit internalsUpdated();
    }
}
void UiVariables::changeWidth()
{
    changeWidth(widthDial->value());
}
void UiVariables::changeScale(int newScale)
{
    if(newScale < 1)
        newScale = 1;

    if(oldScale != newScale)
    {
        int display_width = max( 1, oldWidth / oldScale);

        int display_size = sizeDial->value() / scaleDial->value();
        display_size = max( 1, display_size);
        int newWidth = display_width * newScale;
        widthDial->setValue( newWidth);
        scaleDial->setValue(newScale);
        sizeDial->setMinimum(scaleDial->value() * 500);
        sizeDial->setValue(display_size*newScale);

        widthDial->setSingleStep(newScale);//increment the width step by scaleDial
        widthDial->setMinimum(newScale);
        oldScale = newScale;
        oldWidth = newWidth;
        emit internalsUpdated();
    }
}
void UiVariables::changeScale()
{
    changeScale(scaleDial->value());
}

void UiVariables::changeStart(int start)
{
    if(start != startDial->value())
    {
        startDial->setValue(start);
        emit internalsUpdated();
    }
}

void UiVariables::changeZoom(int zoom)
{
    if(zoom != zoomDial->value())
    {
        zoomDial->setValue(zoom);
        emit internalsUpdated();
    }
}

void UiVariables::changeOffset(int offset)
{
    if(offsetDial != NULL)
    {
        if(offset != offsetDial->value())
        {
            offsetDial->setValue(offset);
//            startDial->setValue(preOffset + offset    );
//            oldOffset
//                    global_start
            emit internalsUpdated();
        }
    }
}

string UiVariables::getColorSetting()
{
    return colorSetting = string("DRuMS");
}

//void UiVariables::changeColorSetting(string newColorSetting)
//{
//    colorSetting = newColorSetting;
//    emit internalsUpdated();
//}
