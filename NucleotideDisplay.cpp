#include "NucleotideDisplay.h"
#include "glwidget.h"
#include <sstream>

/** *******************************************************
NucleotideDisplay was the start of the Skittle Genome Visualization program.
It operates on the simple principle of converting each nucleotide letter
(A, C, G, T) into a color (Black, Red, Green, and Blue) respectively. Each of
these colors is then mapped onto an oversized pixel (a colored square) in a
string that wraps around the same way we read text; left to right, with the
end of the line snapping back to the left.  Changing the start position (ui->startDial)
that is displayed along the sequence allows the user to scroll through a
large genome sequence.  Changing the width of the display (ui->widthDial) allows the user to
find any repetitive patterns that exist in the sequence.  If the width is a
multiple of a repeating sequence, for example ATGATGATGATGATG the repeat will
appear on the Nucleotide Display as vertical bars.  This is because the displayed
repeat wraps around and end up at the same relative position of the repeat on
the next line.

Nucleotide Display's simple method allows researchers to browse through genome
patterns while staying as close to the raw data as possible.  To see more of a
sequence at once, the user can increase the Scale (ui->scaleDial).  Scale is the
number of nucleotides represented by 1 pixel, which defaults to 1:1.  At scales
larger than 1, the program averages the color values of several nucleotides into
one pixel.  The result is a more soft colored pattern that shows the larger distribution
and nucleotide bias of elements along a chromosome.

RepeatMap uses the scale compressed color from NucleotideDisplay in order to calculate its
frequency map above scale = 1.  CylinderDisplay uses the same color scheme as NucleotideDisplay
except arranged in 3D.  HighlightDisplay, RepeatMap, and RepeatOverview all use the same
left to right pixel layout.  It is the default model for how these Graphs should respond to
mouse clicks like "Find" and "Select".  Like all Graphs, it uses TextureCanvas for a display
surface.
**********************************************************/
NucleotideDisplay::NucleotideDisplay(UiVariables* gui, GLWidget* gl)
    :AbstractGraph(gui, gl)
{	
    hidden = false;
    invert = false;

    actionLabel = string("Nucleotide Display");
    actionTooltip = string("Nucleotides as Colors");
    actionData = actionLabel;
}

NucleotideDisplay::~NucleotideDisplay()
{
}

void NucleotideDisplay::calculateOutputPixels()
{
    const char* genome = sequence->c_str() + ui->getStart(glWidget);//TODO: find a safer way to access this
    sequenceToColors(genome);
    loadTextureCanvas();
    upToDate = true;
}

void NucleotideDisplay::sequenceToColors(const char* genome)
{
    outputPixels.clear();
    if( ui->getScale() > 1)
        color_compress();
    else
    {
        for(int i = 0; i < current_display_size(); ++i)
            outputPixels.push_back( glWidget->colors(genome[i]) );
    }
}

void NucleotideDisplay::color_compress()
{
    int r = 0;
    int g = 0;
    int b = 0;
    int tempScale = ui->getScale();
    int end = current_display_size() + ui->getStart(glWidget) - tempScale;
    const string& seq = *sequence;
    int hard_end = sequence->size();
    end = min(end, hard_end);
    for(int i = ui->getStart(glWidget); i < end; )
    {
        for(int s = 0; s < tempScale && i < end; ++s)
        {
            color current = glWidget->colors(seq[i++]);
            r += current.r;
            g += current.g;
            b += current.b;
        }
        outputPixels.push_back(color(r/tempScale, g/tempScale, b/tempScale));
        r = 0;
        g = 0;
        b = 0;
    }
    upToDate = true;
}

/******SLOTS*****/
/**/
