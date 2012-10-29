#ifndef SKITTLEUTIL_H
#define SKITTLEUTIL_H

#include "UtilDrawBar.h"
#include <string>
using std::string;
using std::vector;

namespace SkittleUtil
{
inline int ACGT_num(char n)
{
    if(n == 'A') return 0;
    if(n == 'T') return 3;
    if(n == 'C') return 1;
    if(n == 'G') return 2;
    return 4;//N and All unknown characters
}

inline int olig_num(char n)
{
    if(n == 'A') return 0;
    if(n == 'T') return 3;
    if(n == 'C') return 1;
    if(n == 'G') return 2;
    return -100000;//handles up to 8-mer correctly
}
inline char num_olig(int n)//inverse function of olig_num
{
    if(n == 0) return 'A';
    if(n == 3) return 'T';
    if(n == 1) return 'C';
    if(n == 2) return 'G';
    if(n == 4) return 'N';
    return 'N';
}

inline char complement(char n)
{
    if(n == 'T') return 'A';
    if(n == 'G') return 'C';
    if(n == 'C') return 'G';
    if(n == 'A') return 'T';
    return 'N';//handles up to 8-mer correctly
}

inline vector<int> countNucleotides(const char* genome, int start, int stop)
{
    vector<int> counts(5,0);
    for(int w = start; w < stop; w++)
    {
        ++counts[ACGT_num(genome[w])];
    }
    return counts;
}


inline string trimPathFromFilename(string path)
{
    int startI = path.find_last_of('/');
    int endI = path.size();//path.find_last_of('.');//
    int sizeI = endI - startI;
    return path.substr(startI+1, sizeI-1);
}

inline bool rangeOverlap(int selectionStart, int selectionEnd, int subjectStart, int subjectEnd)
{
    return ((selectionStart >= subjectStart && selectionStart <= subjectEnd)//start in range
            || (selectionEnd >= subjectStart && selectionEnd <= subjectEnd)//end in range
            || (selectionStart < subjectStart && selectionEnd > subjectEnd));//in the middle
}

} // end of namespace SkittleUtil

using namespace SkittleUtil;//this should propagate to any file that includes SkittleUtil.h
#endif // SKITTLEUTIL_H
