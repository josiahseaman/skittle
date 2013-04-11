#include "RepeatMap.h"
#include "glwidget.h"
#include "SkittleUtil.h"
#include <sstream>
#include <QFrame>

/** ***************************************
RepeatMap is designed to make finding tandem repeats much easier than randomly
scrolling around with NucleotideDisplay.  It was the second Graph model designed
and can reveal any tandem repeat with a frequency bewteen 1-250bp.  RepeatMap is
a grey scale Graph, with the y-axis progressing along the length of the sequence
matched to NucleotideDisplay.  The x-axis, on the other hand, is in frequency space.
The first column of pixels on the left represents an offset (or width) of 1.
Columns progressing to the right represent repeats at increasing longer distances
or longer monomer lengths.

The easiest way to think of RepeatMap is as a summary of a series of NucleotideDisplays.
Tandem repeats appear as a series of vertical bars when the NucleotideDisplay is set to
a width matching the frequency of the tandem repeat.  So RepeatMap is looking for vertical
bars at a given width.
For each horizontal line of pixels on the screen, it makes a NucleotideDisplay starting
at a width of one. To get the score, it counts the number of time where the pixel below
 is the same color as the pixel on that line.  There is about a 25% chance of 2 pixels
being the same color if they are not in a repeat.  Inside of a tandem repeat, displayed
at the correct width, there is closer to 90% similarity between the current line and the
line below.  For example, if you have the five character monomer GAACT and the frequency
is 5,10, or 15... then the first character on each line will always be a G. This would mean
a score of 1.0 (100%) and would be displayed as a white pixel in RepeatMap.  The next four
lines will be dark grey, and then the next multiple (10bp) will also be white.  Thus,
tandem repeats appear as a series of white stripes on the RepeatMap, regardless of the
display width.  The size of the gap between white lines is the size of the repeat monomer.

At scale 1, RepeatMap uses a simple equivalence check A==B?.  At scales greater than 1, it
switches to using the color averaged sequence from NucleotideDisplay instead of the base
sequence.  This has the nice property that RepeatMap always computes in constant time, regardless
of scale.  The size of repeats it is scanning for is proportional to the number of nucleotides
(not pixels) on the screen.
This change in behavior requires a change in the equivalence check, since no two stretches of
1,000bp are exactly the same.  Instead, RepeatMap uses a correlation score between the two RGB
values using: double correlate().  This is the same method as above, but more mathematically
sophisticated.
*******************************************/
RepeatMap::RepeatMap(UiVariables* gui, GLWidget* gl)
    :AbstractGraph(gui, gl)
{
    canvas_3mer = NULL;
    barWidth = 20;
    spacerWidth = 2;
    F_width = 250;
    F_start = 1;
    F_height = 1;
    using3merGraph = true;

    freq = vector< vector<float> >();
    for(int i = 0; i < 400; i++)
    {
        freq.push_back( vector<float>(F_width, 0.0) );
    }
    freq_map_count = 0;
    calculate_count = 0;

    actionLabel = string("Repeat Map");
    actionTooltip = string("Graph of possible alignmments");
    actionData = actionLabel;
}

RepeatMap::~RepeatMap()
{
    if(canvas_3mer != NULL)
        delete canvas_3mer;
}

QScrollArea* RepeatMap::settingsUi()
{	
    settingsTab = new QScrollArea();
    settingsTab->setWindowTitle(QString("Repeat Map Settings"));
    QFormLayout* formLayout = new QFormLayout;
    formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    settingsTab->setLayout(formLayout);
    
    QSpinBox* frequencyStartDial = new QSpinBox(settingsTab);
    frequencyStartDial->setMinimum(1);
    frequencyStartDial->setMaximum(100000);
    frequencyStartDial->setSingleStep(10);
    formLayout->addRow("Starting Offset:", frequencyStartDial);

    connect( frequencyStartDial, SIGNAL(valueChanged(int)), this, SLOT(changeFStart(int)));
    connect( this, SIGNAL(fStartChanged(int)), frequencyStartDial, SLOT(setValue(int)));
    connect( this, SIGNAL(fStartChanged(int)), this, SIGNAL(displayChanged()));
    
    QSpinBox* graphWidthDial  = new QSpinBox(settingsTab);
    graphWidthDial->setMinimum(1);
    graphWidthDial->setMaximum(500);
    graphWidthDial->setSingleStep(10);
    graphWidthDial->setValue(F_width);
    formLayout->addRow("Graph Display Width:", graphWidthDial);

    connect( graphWidthDial, SIGNAL(valueChanged(int)), this, SLOT(changeGraphWidth(int)));
    connect( this, SIGNAL(graphWidthChanged(int)), graphWidthDial, SLOT(setValue(int)));
    connect( this, SIGNAL(graphWidthChanged(int)), this, SIGNAL(displayChanged()));

    QCheckBox* find3merButton = new QCheckBox(settingsTab);
    find3merButton->setChecked(true);
    formLayout->addRow("Find 3mer pattern", find3merButton);
    connect( find3merButton, SIGNAL(toggled(bool)), this, SLOT(toggle3merGraph(bool)));

    return settingsTab;
}

void RepeatMap::display()
{
    checkVariables();

    if( !upToDate )
    {
        if((ui->getScale() > 1)&& nuc != NULL)
        {
            nuc->checkVariables();
            if(!nuc->upToDate)
            {
                nuc->calculateOutputPixels();
            }
            int displayWidth = ui->getWidth() / ui->getScale();
            calculate(nuc->outputPixels, displayWidth);
        }
        else
        {
            freq_map();
            if(using3merGraph && ui->getScale() == 1)
            {
                vector<float> scores_3mer = convolution_3mer();
                vector<float> smoothed_scores = lowPassFilter(scores_3mer);
                load_3mer_canvas(smoothed_scores);
            }
        }
    }
    load_canvas();
    glPushMatrix();
    glScaled(1,-1,1);
    if(using3merGraph && ui->getScale() == 1)// && canvas_3mer != NULL)
    {
        canvas_3mer->display();
        glTranslated(barWidth+spacerWidth, 0, 0);
    }
    textureBuffer->display();


    //Draw Red indicator according to Width
    int displayWidth = ui->getWidth() / ui->getScale();

    glColor4f(1,0,0, 1);//red
    glTranslated(displayWidth - F_start, 500, 0);
    glScaled(.5, 500, 1);
    paint_square(point(-1, 0, .25), color(255,0,0));
    paint_square(point(1, 0, .25), color(255,0,0));
    glPopMatrix();

}

void RepeatMap::load_3mer_canvas(vector<float> scores)
{
    vector<color> barGraph;
    for(int y = 0; y < (int)scores.size(); ++y)
    {
        percentageBar(scores[y], barWidth, barGraph);
    }
    if(canvas_3mer != NULL)
        delete canvas_3mer;
    canvas_3mer = new TextureCanvas(barGraph, barWidth);
}

void RepeatMap::link(NucleotideDisplay* nuc_display)
{
    nuc = nuc_display;
}

void RepeatMap::load_canvas()
{
    outputPixels.clear();
    for( int h = 0; h < height(); h++)
    {
        for(int w = 1; w <= F_width; w++)
        {
            int grey = static_cast<int>(  freq[h][w] * 255 );
            outputPixels.push_back( color(grey, grey, grey) );
        }
    }
    storeDisplay(outputPixels, F_width);

    upToDate = true;
}

void RepeatMap::calculateOutputPixels()
{
    freq_map();
    load_canvas();
}

GLuint RepeatMap::render()
{
    if(! upToDate )
    {
        calculateOutputPixels();
    }

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushMatrix();
    glScaled(1,-1,1);
    textureBuffer->display();
    glPopMatrix();
    glEndList();
    upToDate = true;
    return list;
}

void RepeatMap::freq_map()
{
    qDebug() << "Width: " << ui->getWidth() << "\nScale: " << ui->getScale() << "\nStart: " << ui->getStart(glWidget);

    const char* genome = sequence->c_str() + ui->getStart(glWidget);//TODO: find a safer way to access this
    for( int h = 0; h < height(); h++)
    {
        int tempWidth = ui->getWidth();
        int offset = h * tempWidth;
        /*int end = offset+Width-1;
        //NOTE: This statement is just an optional speed up
        if(genome[offset] == 'N' || genome[offset] == 'n' || genome[end] == 'N' || genome[end] == 'n')
        {//check the first and last of the reference string
            for(int w = 1; w <= F_width; w++)
                freq[h][w] = 0;//set whole row to zero
        }
        else
        {*/
        /** This is the core algorithm of RepeatMap.  For each line, for each width,
          check the line below and see if it matches.         */
        for(int w = 1; w <= F_width; w++)//calculate across widths 1-F_width
        {
            int score = 0;
            for(int line_length = 0; line_length < tempWidth; line_length++)
            {
                if(genome[offset + line_length] == genome[offset + w + (F_start-1) + line_length])
                    score += 1; //pixel matches the one above it
            }
            freq[h][w] = float(score) / tempWidth;
        }
    }
    upToDate = true;
}

vector<float> RepeatMap::convolution_3mer()
{
    int reach = 20 * 3;
    vector<float> mask;
    for(int i = 0; i < reach+1; ++i)//create mask
    {/*reach +1 is important to get the last multiple at +6- offset, otherwise the mask does not truly sum to 0 */
        if(i % 3 == 0)
            mask.push_back(1.0);
        else
            mask.push_back(-0.5);
    }
    vector<float> scores;
    for(int y = 0; y < (int)freq.size(); ++y)
    {
        float lineScore = 0.0;
        for(int x = 1; x < (int)mask.size() && x < (int)freq[y].size(); ++x)//we start at 1 to skip self vs. self comparison
        {
            lineScore += mask[x] * freq[y][x];
            //            lineScore += min((float)0.5, mask[x] * freq[y][x]);//the amount that any position can affect is capped because of tandem repeats with 100% similarity
        }
        scores.push_back(lineScore);
    }
    return scores;
}

int RepeatMap::height()
{
    F_height = (((long int)current_display_size()) - (F_start-1)*ui->getScale() - F_width*ui->getScale() )
            / ui->getWidth();

    F_height = max(0, min(400, F_height) );

    return F_height;
}

/******SLOTS*****/
void RepeatMap::changeFStart(int val)
{
    if(updateInt(F_start, val))
        emit fStartChanged(F_start);
}

void RepeatMap::changeGraphWidth(int val)
{
    if(updateInt(F_width, val))
    {
        //freq.clear();
        //freq = vector< vector<float> >();
        for(int i = 0; i < 400; i++)
        {
            freq[i].clear();
        }
        for(int i = 0; i < 400; i++)
        {
            freq[i] = vector<float>(F_width, 0.0) ;
        }

        emit graphWidthChanged(F_width);
    }
}	

void RepeatMap::toggle3merGraph(bool m)
{
    using3merGraph = m;
    invalidate();
}

string RepeatMap::SELECT_MouseClick(point2D pt)
{
    //range check
    if( pt.x < (int)width() && pt.x >= 0 && pt.y <= height() )
    {
        if(using3merGraph)
            pt.x -= barWidth + spacerWidth;
        if(pt.x < 0)//TODO: 3mer mouseText: clicked on the 3mer detector, not freq_map
            return string();

        int percentage = freq[pt.y][pt.x+1] * 100;//+1 because offset 1 is the first pixel [0]
        pt.x *= ui->getScale();
        int index = pt.y * ui->getWidth();
        index = index + ui->getStart(glWidget);
        int index2 = index + pt.x + F_start;
        int w = min( 100, ui->getWidth() );
        if( index2 + w < (int)sequence->size() )
        {
            stringstream ss;
            ss << percentage << "% similarity at Offset "<< pt.x+ F_start;
            ss << "\nIndex: " << index << ": " << sequence->substr(index, w);
            ss << "\nIndex: " << index2<< ": " << sequence->substr(index2, w);
            return ss.str();
        }
    }
    return string();
}

int RepeatMap::getRelativeIndexFromMouseClick(point2D pt)
{
    return getBeginningOfLineFromMouseClick( pt);
}

/***Correlation***/
vector<point> RepeatMap::bestMatches()
{
    //calculate(color_avgs, width() / ui->getScale());

    vector<point> best_matches;
    for(int h =0; h < height(); h++)
    {
        float best_score = 0;
        int best_freq = 0;
        //if(freq[h][1] != 0)//N's block
        {
            for(int w = 1; w <= F_width; w++)
            {
                float curr = freq[h][w];
                if( curr * .90 > best_score)  //new value must beat old by at least 10%
                {
                    best_score = curr;
                    best_freq = w;
                }
            }
        }
        if(best_freq == 0) best_freq = 1;//NNNNN's show up as 0 and causes crash
        best_matches.push_back( point(best_score, (float)best_freq, 0) );
    }
    return best_matches;
}

void RepeatMap::calculate(vector<color>& img, int pixelsPerSample)//constructs the frequency map
{
    //display_size = img.size();
    checkVariables();
    //glWidget->print("Calculate(): ", ++calculate_count);
    for( int h = 0; h < height(); h++)
    {
        int offset = h * pixelsPerSample;
        for(int w = 1; w <= F_width; w++)//calculate across widths 1-F_width
        {
            freq[h][w] = .5 * (1.0 + correlate(img, offset, offset + w + F_start, pixelsPerSample));
        }
    }
    upToDate = true;
}

double RepeatMap::correlate(vector<color>& img, int beginA, int beginB, int pixelsPerSample)//calculations for a single pixel
{//correlation will be a value between -1 and 1 representing how closley related 2 sequences are
    //calculation variables!!!  should all be double to prevent overflow
    double N = pixelsPerSample;
    int AVal;		int BVal;

    double ARedsum = 0,	AGreensum = 0,		ABluesum = 0;   //our tuple of color sums
    double BRedsum = 0,	BGreensum = 0,		BBluesum = 0;   //our tuple of color sums
    double ASquaredRed = 0,	ASquaredGreen = 0,	ASquaredBlue = 0;  //this is Aij^2
    double BSquaredRed = 0,	BSquaredGreen = 0,	BSquaredBlue = 0;  //this is Bij^2
    double ABRed = 0, 	ABGreen =0,		ABBlue =0;	//this is A[]*B[]

    for (int k = 0; k < pixelsPerSample; k++)
    {//3 color shades RGB,  2 samples A and B
        //reds
        color A = img[beginA + k];
        color B = img[beginB + k];
        AVal = A.r;							BVal = B.r;
        ARedsum += AVal;					BRedsum += BVal;
        ASquaredRed += (AVal*AVal);			BSquaredRed += (BVal*BVal);
        ABRed += (AVal * BVal);
        //Greens
        AVal = A.g;							BVal = B.g;
        AGreensum += AVal;					BGreensum += BVal;
        ASquaredGreen += (AVal*AVal);		BSquaredGreen += (BVal*BVal);
        ABGreen += (AVal * BVal);
        //Blues
        AVal = A.b;							BVal = B.b;
        ABluesum += AVal;					BBluesum += BVal;
        ASquaredBlue += (AVal*AVal);		BSquaredBlue += (BVal*BVal);
        ABBlue += (AVal * BVal);

    }

    //calculation time
    double AbarRed = 0,	AbarGreen = 0,			AbarBlue = 0;  //A-tuple for color means
    double BbarRed = 0,	BbarGreen = 0,			BbarBlue = 0;  //B-tuple for color means
    AbarRed = ARedsum / N;	AbarGreen = AGreensum / N;	AbarBlue = ABluesum / N;
    BbarRed = BRedsum / N;	BbarGreen = BGreensum / N;	BbarBlue = BBluesum / N;

    double numerator_R = ABRed   - BbarRed   * ARedsum   - AbarRed * BRedsum     + AbarRed   * BbarRed   * N;
    double numerator_G = ABGreen - BbarGreen * AGreensum - AbarGreen * BGreensum + AbarGreen * BbarGreen * N;
    double numerator_B = ABBlue  - BbarBlue  * ABluesum  - AbarBlue * BBluesum   + AbarBlue  * BbarBlue  * N;

    double denom_R1 = (sqrt(ASquaredRed   - ((ARedsum   * ARedsum)  /N)));
    double denom_R2 = (sqrt(BSquaredRed   - ((BRedsum   * BRedsum)  /N)));
    double denom_G1 = (sqrt(ASquaredGreen - ((AGreensum * AGreensum)/N)));
    double denom_G2 = (sqrt(BSquaredGreen - ((BGreensum * BGreensum)/N)));
    double denom_B1 = (sqrt(ASquaredBlue  - ((ABluesum  * ABluesum) /N)));
    double denom_B2 = (sqrt(BSquaredBlue  - ((BBluesum  * BBluesum) /N)));

    double backup = sqrt(1 - (1/N));//if we have 0 instances of a color it will be / 0  div0
    if(denom_R1 == 0) denom_R1 = backup;
    if(denom_R2 == 0) denom_R2 = backup;
    if(denom_G1 == 0) denom_G1 = backup;
    if(denom_G2 == 0) denom_G2 = backup;
    if(denom_B1 == 0) denom_B1 = backup;
    if(denom_B2 == 0) denom_B2 = backup;

    double answer_R = numerator_R / (denom_R1 * denom_R2);
    double answer_G = numerator_G / (denom_G1 * denom_G2);
    double answer_B = numerator_B / (denom_B1 * denom_B2);

    return (answer_R + answer_G + answer_B)/3;//return the average of RGB correlation
}

int RepeatMap::width()
{
    int w = F_width;
    if(using3merGraph && ui->getScale() == 1)
        w += barWidth + spacerWidth;
    return w;
}
