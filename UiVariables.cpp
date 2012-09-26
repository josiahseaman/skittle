#include <QtGui/QTextEdit>
#include <QtGui/QSpinBox>
#include <QString>
#include <sstream>
#include <algorithm>
#include "UiVariables.h"

using std::max;
/** *******************************
This class is a container class for the set of 5 global dials that affect all
Graph visualizations.  Those dials are: start, width, size, scaleDial, and zoom.
It also contains a pointer to the local offsetDial for multiple windows.
Finally, UiVariables has a pointer to the textArea of "Information Display"
and convenience functions for printing out information to the text area.  This
is useful for writing debugging notes since cout is generally unavailable.
You should also use the print functions to provide data to the user in a format
that can be copy and pasted without the need to write a file.
**********************************/

UiVariables::UiVariables()
{
	textArea = NULL;
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    offsetDial = NULL;
    oldScale = 1;
    oldWidth = 128;
}

UiVariables::UiVariables(QTextEdit* text)
{
	textArea = text;
	textArea->toPlainText();
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    offsetDial = NULL;
    oldScale = 1;
    oldWidth = 128;
}

UiVariables::UiVariables(const UiVariables& copy)
{
	textArea  = copy.textArea;
    sizeDial  = copy.sizeDial;
    widthDial = copy.widthDial;
    startDial = copy.startDial;
    scaleDial = copy.scaleDial;
    zoomDial  = copy.zoomDial;
    offsetDial= copy.offsetDial;
    oldWidth  = copy.oldWidth;
    oldScale  = copy.oldScale;
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
    }
}
void UiVariables::changeScale()
{
    changeScale(scaleDial->value());
}


