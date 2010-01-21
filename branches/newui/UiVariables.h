#ifndef UI_VARS
#define UI_VARS

#include <QtGui/QSpinBox>
#include <QtGui/QScrollBar>

struct UiVariables
{
	QScrollBar* verticalScrollBar;
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;


UiVariables()
{
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
}

};
#endif
