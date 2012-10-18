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
    if(n == 'A') return 2;
    if(n == 'T') return 3;
    if(n == 'C') return 0;
    if(n == 'G') return 1;
    if(n == '3') return 5;//3-mer pattern
    return 4;//N and All unknown characters
}

inline string trimPathFromFilename(string path)
{
    int startI = path.find_last_of('/');
    int endI = path.size();//path.find_last_of('.');//
    int sizeI = endI - startI;
    return path.substr(startI+1, sizeI-1);
}


}

using namespace SkittleUtil;//this should propagate to any file that includes SkittleUtil.h
#endif // SKITTLEUTIL_H
