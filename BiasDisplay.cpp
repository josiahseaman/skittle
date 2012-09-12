#include "BiasDisplay.h"
#include "glwidget.h"

BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
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
//    if(scale > 1)
//        color_compress();
//    else
    {
        for(int h = 0; h < height(); h++)//TODO: This code could be replaced with the code from OligomerDisplay freq_map()
        {
            char letters[] = {'A', 'C', 'G', 'T'};
            vector<int> counts(4,0);
            for(int w = 0; w < Width; w++)
            {
                ++counts[ACGT_num(genome[h*Width + w])];
            }
            for(int c = 0; c < counts.size(); ++c){
                for(int letterCounter = 0; letterCounter < counts[c]; ++letterCounter){
                    nucleotide_colors.push_back( glWidget->colors(letters[c]) );
                }
            }

        }
//        for(int i = 0; i < display_size; ++i)
//            nucleotide_colors.push_back( glWidget->colors(genome['C']) );//TODO: Optimize pointer function call
    }
}
