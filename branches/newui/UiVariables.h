#ifndef UI_VARS
#define UI_VARS

#include <QtGui/QSpinBox>
#include <QtGui/QScrollBar>

class UiVariables
{
public:
	QScrollBar* horizontalScrollBar;
	QScrollBar* verticalScrollBar;
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* oligDial;


UiVariables()
{
	horizontalScrollBar = NULL;
	verticalScrollBar = NULL;
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
}

};
#endif
