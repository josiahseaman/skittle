#ifndef UI_VARS
#define UI_VARS

#include <QtGui/QSpinBox>
#include <QtGui/QScrollBar>

class UiVariables
{
public:
	QScrollBar* verticalScrollBar;
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;


UiVariables()
{
	verticalScrollBar = NULL;
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
}

};
#endif
