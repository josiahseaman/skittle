#include "BiasDisplay.h"
#include "glwidget.h"

BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
:NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
    display_width = 120;
}

void BiasDisplay::load_nucleotide()
{
    const char* genome = sequence->c_str() + ui->startDial->value();
    sequenceToColors(genome);

    loadTextureCanvas(true);
    upToDate = true;
}

void BiasDisplay::sequenceToColors(const char* genome)
{
    nucleotide_colors.clear();
    int max_bar_width = display_width / 3;

    for(int h = 0; h < height(); h++)
    {
        vector<int> counts = countNucleotides(genome + h*Width);//TODO: The counting code could be replaced with the code from OligomerDisplay freq_map()
        vector<int> bar_sizes;
        float remainder = 0.0;
        float floating_sum = 0.0;
        for(int c = 0; c < (int)counts.size(); ++c)
        {
            float barSize = (float)counts[c] / Width * max_bar_width + .00001;  //normalize the size of the bar to display_width
            floating_sum += barSize;
            barSize += remainder;
            remainder = floating_sum - (int)(floating_sum + .5);
            bar_sizes.push_back((int)(barSize+.5));
        }
        vector<color> bar = drawJustifiedBar(bar_sizes);
        nucleotide_colors.insert(nucleotide_colors.end(), bar.begin(), bar.end()  );
    }
    return;
}

vector<int> BiasDisplay::countNucleotides(const char* genome)
{
    vector<int> counts(4,0);
    for(int w = 0; w < Width; w++)
    {
        ++counts[ACGT_num(genome[w])];
    }
    return counts;
}


vector<color>& vector_append(vector<color>& A, vector<color>& B)
{
    for(int i = 0; i < (int)B.size(); ++i)
        A.push_back(B[i]);
    return A;
}

vector<color> BiasDisplay::drawJustifiedBar(vector<int> alphabetic_sizes)
{
    char order[] = {'C','G','A','T'};
    if(alphabetic_sizes.size() != 4){
        ui->print("Error: Nucleotide Bias line size is not 4.");
        return vector<color>();
    }
    //reorder sizes to match order[]
    vector<int> sizes(4, 0);
    for(int i = 0; i < (int)sizes.size(); ++i)
        sizes[i] = alphabetic_sizes[ACGT_num(order[i])];

    vector<color> line;

    for(int position = 0; position < 4; ++position)
    {
        int size = sizes[position];

        int filler_size = 0;
        bool rightJustified = true;
        switch(position)//this is the order in which the nucleotides are displayed
        {//the two pointing outwards need twice the margin of the inner two because
        //the inner two can overlap each other.
        case 0:
            filler_size = display_width / 3 - size;
            rightJustified = true;
            break;
        case 1:
            filler_size = display_width / 3 - size - sizes[2];//overflow from the next letter
            rightJustified = false;
            break;
        case 2:
            filler_size = 0;//filler already handled by position 1
            rightJustified = true;
            break;
        case 3:
            filler_size = display_width / 3 - size;
            rightJustified = false;
            break;

        }
        color barColor = glWidget->colors(order[position]);
        vector<color> filler( max(0,filler_size), color(0.5));
        vector<color> bar(size, barColor);
        if(rightJustified)
        {
            vector_append(line, filler);
            vector_append(line, bar);
//            filler.insert(filler.end(), bar.begin(),bar.end());
        }
        else{
            vector_append(line, bar);
            vector_append(line, filler);
//            bar.insert(bar.end(),filler.begin(), filler.end());
        }
    }
    return line;
}


/** Overrides the AbstractGraph::width() because Bias has a fixed size */
int BiasDisplay::width()
{
    return display_width;
}

