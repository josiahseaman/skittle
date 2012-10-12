#include "BiasDisplay.h"
#include "glwidget.h"

BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
    max_bar_width = 40;
}

void BiasDisplay::load_nucleotide()
{
    const char* genome = sequence->c_str() + ui->startDial->value();//TODO: find a safer way to access this
    sequenceToColors(genome);

    loadTextureCanvas(true);
    upToDate = true;
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
        vector<color> bar = drawJustifiedBar(bar_sizes, max_bar_width, glWidget);
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

/** Overrides the AbstractGraph::width() because Bias has a fixed size */
int BiasDisplay::width()
{
    return max_bar_width * 3;
}

