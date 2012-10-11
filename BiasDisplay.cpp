#include "BiasDisplay.h"
#include "glwidget.h"

BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
    f3_bar_width = 20;
    max_bar_width = 40;
    spacer_width = 5;
}

void BiasDisplay::load_nucleotide()
{
    const char* genome = sequence->c_str() + ui->startDial->value();//TODO: find a safer way to access this
    sequenceToColors(genome);

    loadTextureCanvas(true);
    upToDate = true;
}

vector<color>& vector_append(vector<color>& A, vector<color>& B)
{
    for(int i = 0; i < (int)B.size(); ++i)
        A.push_back(B[i]);
    return A;
}

void BiasDisplay::sequenceToColors(const char* genome)
{
    nucleotide_colors.clear();
    int tempWidth = ui->widthDial->value();

    for(int h = 0; h < height(); h++)
    {
        vector<int> counts = countNucleotides(genome + h*tempWidth);//TODO: The counting code could be replaced with the code from OligomerDisplay freq_map()
        vector<int> bar_sizes;
        float remainder = 0.0;
        float floating_sum = 0.0;
        for(int c = 0; c < (int)counts.size(); ++c)
        {
            float barSize = (float)counts[c] / tempWidth * max_bar_width + .00001;  //normalize the size of the bar to display_width
            floating_sum += barSize;
            barSize += remainder;
            remainder = floating_sum - (int)(floating_sum + .5);
            bar_sizes.push_back((int)(barSize+.5));
        }
        vector<color> bar = drawJustifiedBar(bar_sizes);
        //place spacer
        bar = drawBar(0, spacer_width, color(0,0,0), false, bar);

        float threeScore = count_3merPattern(genome + h * tempWidth);
        color threeColor(70,70,255);
        if (threeScore > 1.0)
            threeColor = color(200,0,150);

        int temp = max(0, (int)( (threeScore) * f3_bar_width) );
        int threeMerBar = min( f3_bar_width, temp);
        int filler_size = f3_bar_width - threeMerBar;
        bar = drawBar(threeMerBar, filler_size, threeColor, false, bar);

        nucleotide_colors.insert(nucleotide_colors.end(), bar.begin(), bar.end()  );
    }
    return;
}

vector<int> BiasDisplay::countNucleotides(const char* genome)
{
    vector<int> counts(5,0);
    int tempWidth = ui->widthDial->value();
    for(int w = 0; w < tempWidth; w++)
    {
        ++counts[ACGT_num(genome[w])];
    }
    return counts;
}

float BiasDisplay::count_3merPattern( const char* genome)
{
    int F_width = 3;
    vector<float> freq(F_width+1,0);
    int tempWidth = ui->widthDial->value();
    /** This is the core algorithm of RepeatMap.  For each line, for each width,
              check the line below and see if it matches.         */
    for(int w = 1; w <= F_width; w++)//calculate across widths 1-F_width
    {
        int score = 0;
        for(int line_length = 0; line_length < tempWidth; line_length++)
        {
            if(genome[line_length] == genome[w + line_length])
                score += 1; //pixel matches the one above it
        }
        freq[w] = float(score) / tempWidth;
    }
    float background = (freq[1] + freq[2]) / 2.0;
    float normalized = (freq[3]-background) / (1.0 - background);
    normalized = normalized / 0.2;//ceiling is not 1.0
    return normalized;
}

vector<color>& BiasDisplay::drawBar(int size, int filler_size, color barColor, bool rightJustified, vector<color>& line)
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

vector<color> BiasDisplay::drawJustifiedBar(vector<int> alphabetic_sizes)
{
    char r[] = {'C','G','A','T','N'};
    vector<char> order(r, r + 5);//this should be the size of the above array.
    if(alphabetic_sizes.size() != order.size()){
        ui->print("Error: Nucleotide Bias line size is not 5.");
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
        color barColor = glWidget->colors(order[position]);
        line = drawBar(size, filler_size, barColor, rightJustified, line);
    }
    return line;
}

/** Overrides the AbstractGraph::width() because Bias has a fixed size */
int BiasDisplay::width()
{
    return max_bar_width * 3 + f3_bar_width + spacer_width;
}

