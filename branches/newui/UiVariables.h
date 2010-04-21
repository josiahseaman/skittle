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
    QSpinBox* oligDial;


UiVariables()
{
    sizeDial = NULL;
    widthDial = NULL;
    startDial = NULL;
    scaleDial = NULL;
    zoomDial = NULL;
    oligDial = NULL;
}

UiVariables(const UiVariables& copy)
{
    sizeDial  = copy.sizeDial;
    widthDial = copy.widthDial;
    startDial = copy.startDial;
    scaleDial = copy.scaleDial;
    zoomDial  = copy.zoomDial;
    oligDial  = copy.oligDial;
}

};
#endif
