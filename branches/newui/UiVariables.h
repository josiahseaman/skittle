#ifndef UI_VARS
#define UI_VARS

#include <QtGui/QSpinBox>
#include <QtGui/QScrollBar>

class UiVariables
{
public:
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* offsetDial;


UiVariables()
{
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    offsetDial = NULL;
}

UiVariables(const UiVariables& copy)
{
    sizeDial  = copy.sizeDial;
    widthDial = copy.widthDial;
    startDial = copy.startDial;
    scaleDial = copy.scaleDial;
    zoomDial  = copy.zoomDial;
    offsetDial= copy.offsetDial;
}

};
#endif
