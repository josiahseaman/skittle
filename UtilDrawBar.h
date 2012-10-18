#ifndef UTILDRAWBAR_H
#define UTILDRAWBAR_H

#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include "BasicTypes.h"
#include "glwidget.h"
#include "SkittleUtil.h"

using std::vector;

namespace SkittleUtil
{
vector<float> lowPassFilter(vector<float> scores);
vector<color>& percentageBar(float percentage, int barWidth, vector<color> &line, bool rightJustified = false);
vector<color>& drawBar(int size, int filler_size, color barColor, bool rightJustified, vector<color>& line);
vector<color>  drawJustifiedBar(vector<int> alphabetic_sizes, int max_bar_width, GLWidget* gl);
vector<color>& vector_append(vector<color>& A, vector<color>& B);
}

#endif // UTILDRAWBAR_H
