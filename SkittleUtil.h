#ifndef SKITTLEUTIL_H
#define SKITTLEUTIL_H

#include "UtilDrawBar.h"

namespace SkittleUtil
{
inline int ACGT_num(char n)
{
    if(n == 'A') return 2;
    if(n == 'T') return 3;
    if(n == 'C') return 0;
    if(n == 'G') return 1;
    if(n == '3') return 5;//3-mer pattern
    return 4;//N and All unknown characters
}
}

#endif // SKITTLEUTIL_H
