#ifndef BIASDISPLAY_H
#define BIASDISPLAY_H
#include "NucleotideDisplay.h"
#include <string>
#include <vector>

using namespace std;


class BiasDisplay : public NucleotideDisplay
{
    Q_OBJECT
public:
    int display_width;

    BiasDisplay(UiVariables* gui, GLWidget* gl);
    void load_nucleotide();
    void sequenceToColors(const char* genome);
    vector<int> countNucleotides(const char* genome);
    vector<color> drawJustifiedBar(vector<int> alphabetic_sizes);
    virtual int width();
    
    inline int ACGT_num(char n)
    {
        if(n == 'A') return 2;
        if(n == 'T') return 3;
        if(n == 'C') return 0;
        if(n == 'G') return 1;
        return 4;//N and All unknown characters
    }
signals:
    
public slots:
    
};

#endif // BIASDISPLAY_H
