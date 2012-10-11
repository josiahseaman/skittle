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
    int f3_bar_width;
    int max_bar_width;
    int spacer_width;

    BiasDisplay(UiVariables* gui, GLWidget* gl);
    void load_nucleotide();
    void sequenceToColors(const char* genome);
    vector<int> countNucleotides(const char* genome);
    float count_3merPattern( const char* genome);
    vector<color>& drawBar(int size, int filler_size, color barColor, bool rightJustified, vector<color>& line);
    vector<color>  drawJustifiedBar(vector<int> alphabetic_sizes);

    virtual int width();
    
    inline int ACGT_num(char n)
    {
        if(n == 'A') return 2;
        if(n == 'T') return 3;
        if(n == 'C') return 0;
        if(n == 'G') return 1;
        if(n == '3') return 5;//3-mer pattern
        return 4;//N and All unknown characters
    }
signals:
    
public slots:
    
};

#endif // BIASDISPLAY_H
