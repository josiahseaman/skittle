#include "BiasDisplay.h"
#include "glwidget.h"

BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
    display_width = 40;
}

QScrollArea* BiasDisplay::settingsUi()
{
    return NULL;
}


void BiasDisplay::load_nucleotide()
{
    const char* genome = sequence->c_str() + nucleotide_start;
    sequenceToColors(genome);

    loadTextureCanvas();
    upToDate = true;
}

void BiasDisplay::sequenceToColors(const char* genome)
{
    nucleotide_colors.clear();

    for(int h = 0; h < height(); h++)//TODO: This code could be replaced with the code from OligomerDisplay freq_map()
    {
        char letters[] = {'A', 'C', 'G', 'T'};
        vector<int> counts(4,0);
        for(int w = 0; w < Width; w++)
        {
            ++counts[ACGT_num(genome[h*Width + w])];
        }
        float remainder = 0.0;
        float floating_sum = 0.0;
        for(int c = 0; c < (int)counts.size(); ++c)
        {
            float barSize = (float)counts[c] / Width * display_width + .000001;  //normalize the size of the bar to display_width
            floating_sum += barSize;
            barSize += remainder;
            remainder = floating_sum - (int)(floating_sum + .5);

            vector<color> bar = drawBar((int)(barSize+.5), glWidget->colors(letters[c]) );
            nucleotide_colors.insert(nucleotide_colors.end(), bar.begin(), bar.end()  );
        }
    }

}

vector<color> BiasDisplay::drawBar(int size, color barColor)
{
    vector<color> bar;
    for(int letterCounter = 0; letterCounter < size; ++letterCounter)
    {
        bar.push_back( barColor );
    }
    return bar;
}

/** Overrides the AbstractGraph::width() because Bias has a fixed size */
int BiasDisplay::width()
{
    return display_width;
}

