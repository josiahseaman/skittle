'''
Created on Dec 14, 2012

@author: Josiah
'''
import math

from SkittleGraphTransforms import pearsonCorrelation
from SkittleCore.models import chunkSize
from models import SimilarityHeatMapState
import OligomerUsage
from SkittleCore.GraphRequestHandler import registerGraph
from PixelLogic import twoSidedSpectrumColoring


registerGraph('s', "Similarity Heatmap", __name__, False, False, 0.4, helpText='''This graph is a heatmap that shows how similar 
each row is to every other row on the screen.  Red represents positive correlation, blue is negative, with black being neutral.  
The red blue spectrum is normalized to account for baseline correlation from genome wide patterns. 
The structure of a heatmap is diagonally symmetrical.
The diagonal red line is self compared with self. 
Each pixel represents a comparison between two other lines.
To see which lines are involved in a comparison trace one line straight down to the diagonal and another line to the left.
The Similarity Heatmap is useful to visualize the
blocks of similar code found in the genome, such as large tandem repeats, and isochores at all scales. 
The patterns in Similarity Heatmap correlate strongly with those generated from Hi-C experiments to map chromosome territories.''')


def prettyPrint(heatMap):
    for line in heatMap:
        print
        for e in line:
            if isinstance(e, float):
                print round(e, 2), ', ',
            else:
                print e, ', ',
    print #newline


def medianFromAllLines(heatMap):
    normalizationList = []
    for line in heatMap:
        normalizationList += line
    normalizationList.sort()
    return normalizationList[len(normalizationList) / 2]


def calculateOutputPixels(state, heatMapState=SimilarityHeatMapState()):
    state.readFastaChunks()
    width = 300
    while len(state.seq) < (
            chunkSize * state.scale) + width * state.nucleotidesPerLine(): #all starting positions plus the maximum reach from the last line
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    oligVectors = OligomerUsage.calculateOutputPixels(state)
    heatMap = [[None for x in range(width)] for y in range(height)]

    for y in range(len(heatMap)):
        for x in range(0, len(heatMap[y])):
            if x == 0:
                heatMap[y][x] = 1.0 #don't bother calculating self:self
            elif x + y < len(oligVectors):#account for second to last chunk
                heatMap[y][x] = pearsonCorrelation(oligVectors[y], oligVectors[y + x])

                #TODO: the mirroring probably needs changing given the new heatmap png layout
                #    if heatMapState.useRowColumnCorrelation:
                #        mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
                ##        prettyPrint(heatMap)
                ##        print
                #        heatMap = rowColumnCorrelation(heatMap)
    median = 0.0 #medianFromAllLines(heatMap)

    pixels = twoSidedSpectrumColoring(heatMap, median)
    return pixels

