#include "BiasDisplay.h"
#include "glwidget.h"
#include "SkittleUtil.h"
#include <sstream>

/** **********************************************
  BiasDisplay shows the line per line usage bias of the
  four nucleotides: A,C,G,T.  It does this by counting all the letters
  then displaying it in the form of a bar graph.  The bar graph
  uses the "centered" design by Marshall.  C&G are on the left so that
  the user can see CG vs AT bias.  G&A are facing inwards to accent any
  poly-purine tracks indicating Triplex DNA.  The ragged appearance of
  the bar graphs is accomplished by inserting 50% grey pixels that blend in
  with the background, giving bar graphs a fake transparent section.
  The inward facing G and A actually share the same bar graph space, totalling
  100% sequence coverage.  They will never collide because  you can't have
  for example 70% G's AND 70% A's at the same time.  Because of the overlap,
  the justifiedBarGraph is 300% wide (4-1)*100%.
*******************************************************/
BiasDisplay::BiasDisplay(UiVariables* gui, GLWidget* gl)
    :NucleotideDisplay(gui, gl)
{
    hidden = true;
    actionLabel = string("Nucleotide Bias");
    actionTooltip = string("Displays bar graphs of nucleotide bias per line");
    actionData = actionLabel;
    max_bar_width = 40;
}

void BiasDisplay::calculateOutputPixels()
{
    const char* genome = sequence->c_str() + ui->getStart(glWidget);//TODO: find a safer way to access this, this would require modifying AbstractGraph::calculateOutputPixels() and all child classes
    sequenceToColors(genome);

    loadTextureCanvas(true);
    upToDate = true;
}

void BiasDisplay::sequenceToColors(const char* genome)
{
    outputPixels.clear();
    int tempWidth = ui->widthDial->value();

    for(int h = 0; h < height(); h++)
    {
        vector<int> counts = countNucleotides(genome,  h*tempWidth, h*tempWidth+tempWidth );
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
        outputPixels.insert(outputPixels.end(), bar.begin(), bar.end()  );
    }
    return;
}

/** Overrides the AbstractGraph::width() because Bias has a fixed size */
int BiasDisplay::width()
{
    return max_bar_width * 3;
}

int BiasDisplay::getRelativeIndexFromMouseClick(point2D pt)
{
    return getBeginningOfLineFromMouseClick( pt);
}

string BiasDisplay::SELECT_MouseClick(point2D pt)
{
    if( pt.x < width() && pt.x >= 0  )
    {
        int tempWidth = ui->widthDial->value();
        int index = pt.y * tempWidth;
        index = index + ui->getStart(glWidget);
        int end = index + tempWidth;
        const char* genome = sequence->c_str();
        vector<int> counts = countNucleotides(genome,  index, end );
        char r[] = {'C','G','A','T','N'};
        float col = pt.x / (float)max_bar_width;
        if(col >= 1.5)//if it's more than halfway through the middle column
            col += 1.0;//add one column to compensate for G/A sharing the same space.
        int column = (int)col;//drops the floating point
        char character = r[column];
        float count = counts[ACGT_num(character)];
        int percent = count / tempWidth * 100.0;
        stringstream ss;
        ss << character << "  Count: " << counts[ACGT_num(character)] << " = "
           << percent << "%" << " at Index: "<< index << " - "<< end;

        return ss.str();
    }
    return string();
}
