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
    int f3_bar_width;
    int max_bar_width;
    int spacer_width;

    BiasDisplay(UiVariables* gui, GLWidget* gl);
    void load_nucleotide();
    void sequenceToColors(const char* genome);
    vector<int> countNucleotides(const char* genome);
    float count_3merPattern( const char* genome);

    virtual int width();
    
signals:
    
public slots:
    
};

#endif // BIASDISPLAY_H
