#ifndef BIASDISPLAY_H
#define BIASDISPLAY_H
#include "NucleotideDisplay.h"
#include "SkittleUtil.h"
#include <string>
#include <vector>

using namespace std;
using namespace SkittleUtil;


class BiasDisplay : public NucleotideDisplay
{
    Q_OBJECT
public:
    int max_bar_width;

    BiasDisplay(UiVariables* gui, GLWidget* gl);
    void calculateOutputPixels();
    void sequenceToColors(const char* genome);
    vector<int> countNucleotides(const char* genome);

    virtual int width();
    
signals:
    
public slots:
    
};

#endif // BIASDISPLAY_H
