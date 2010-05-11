#include <QtGui/QTextEdit>
#include <QtGui/QSpinBox>
#include <QString>
#include "UiVariables.h"

/*UiVariables::UiVariables()
{
	textArea = NULL;
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    offsetDial = NULL;
}*/

UiVariables::UiVariables(QTextEdit* text)
{
	textArea = text;
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    offsetDial = NULL;
}
/*
UiVariables::UiVariables(const UiVariables& copy)
{
    sizeDial  = copy.sizeDial;
    widthDial = copy.widthDial;
    startDial = copy.startDial;
    scaleDial = copy.scaleDial;
    zoomDial  = copy.zoomDial;
    offsetDial= copy.offsetDial;
}*/

void UiVariables::print(char const * s)
{
	textArea->append(QString(s));	
}

/*void UiVariables::print(std::string s)
{
	textArea->append(QString(s.c_str()));	
}*/

void UiVariables::printHtml(std::string s)
{
	//QTextCursor cursor = textArea->textCursor();
	//cursor.movePosition(QTextCursor::End);
	textArea->insertHtml(QString(s.c_str()));
}

/*
void UiVariables::print(const char* s, int num)
{
	stringstream ss1;
	ss1 << s << num;

	textArea->append(QString( ss1.str().c_str() ));	
}
void UiVariables::print(int num1, int num2)
{
	stringstream ss1;
	ss1 << num1 << ", " << num2;

	textArea->append(QString( ss1.str().c_str() ));	
}*/

