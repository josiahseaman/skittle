#include "UtilDrawBar.h"

namespace SkittleUtil
{

vector<float> lowPassFilter(vector<float> scores)
{
    vector<float> smoothed_scores(scores.size(), 0.0);
    for(int i = 1; i < (int)scores.size()-1; ++i)
    {
        smoothed_scores[i] = (scores[i-1] + scores[i] + scores[i+1]) / 2.0;// 3/2 boosts the values after smoothing
    }
    if(!scores.empty())//fill in first and last edge conditions
    {
        smoothed_scores[0] = scores[0];
        smoothed_scores[scores.size()-1] = scores[scores.size()-1];
    }
    return smoothed_scores;
}


vector<color>& percentageBar(float percentage, int barWidth, vector<color>& line, bool rightJustified)
{
//    if(barColor == NULL)
//    {
        color barColor = color(65,102,198);
        if (percentage > 1.0)
            barColor = color(112,0,174);
//    }

    int size = min( barWidth, max(0, (int)(percentage * barWidth)));
    int filler_size = barWidth - size;
    line = drawBar(size, filler_size, barColor, rightJustified, line);
    return line;
}

vector<color>& drawBar(int size, int filler_size, color barColor, bool rightJustified, vector<color>& line)
{
    vector<color> filler( max(0,filler_size), color(0.5));
    vector<color> bar(size, barColor);
    if(rightJustified)
    {
        vector_append(line, filler);
        vector_append(line, bar);
    }
    else{
        vector_append(line, bar);
        vector_append(line, filler);
    }
    return line;
}

vector<color> drawJustifiedBar(vector<int> alphabetic_sizes, int max_bar_width, GLWidget* gl)
{
    char r[] = {'C','G','A','T','N'};
    vector<char> order(r, r + 5);//this should be the size of the above array.
    if(alphabetic_sizes.size() != order.size()){
        throw "Error: Nucleotide Bias line size is not 5.";
        return vector<color>();
    }
    //reorder sizes to match order[]
    vector<int> sizes(alphabetic_sizes.size(), 0);
    for(int i = 0; i < (int)sizes.size(); ++i)
        sizes[i] = alphabetic_sizes[ACGT_num(order[i])];

    vector<color> line;

    for(int position = 0; position < 5; ++position)
    {
        int size = sizes[position];

        int filler_size = 0;
        bool rightJustified = true;
        switch(position)//this is the order in which the nucleotides are displayed
        {//the two pointing outwards need twice the margin of the inner two because
        //the inner two can overlap each other.
        case 0:
            filler_size = max_bar_width - size;
            rightJustified = true;
            break;
        case 1:
            filler_size = max_bar_width - size - sizes[2];//overflow from the next letter
            rightJustified = false;
            break;
        case 2:
            filler_size = 0;//filler already handled by position 1
            rightJustified = true;
            break;
        case 3:
            filler_size = max_bar_width - size - sizes[4];
            rightJustified = false;
            break;
        case 4://N's
            filler_size = 0;
            rightJustified = true;
            break;
        }
        color barColor = gl->colors(order[position]);
        line = drawBar(size, filler_size, barColor, rightJustified, line);
    }
    return line;
}

vector<color>& vector_append(vector<color>& A, vector<color>& B)
{
    for(int i = 0; i < (int)B.size(); ++i)
        A.push_back(B[i]);
    return A;
}
}
