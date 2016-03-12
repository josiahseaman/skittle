'''
Created on March 11, 2016

@author: Josiah
'''
import math

from SkittleGraphTransforms import pearsonCorrelation, reverseComplement, generateExhaustiveOligomerList, hasDepth
from SkittleCore.models import chunkSize
from models import SimilarityHeatMapState
import OligomerUsage
from SkittleCore.GraphRequestHandler import registerGraph
from PixelLogic import twoSidedSpectrumColoring


registerGraph('c', "Reverse Complement Map", __name__, False, False, 0.4, helpText='''This graph is a heatmap that shows
nearby reverse complementary sequences.  Look for short diagonal lines perpendicular to the main axis (upper left to
bottom right).
The structure of a heatmap is diagonally symmetrical.
The diagonal red line is self compared with self.
Each pixel represents a comparison between two other lines.
To see which lines are involved in a comparison trace one line straight down to the diagonal and another line to the left.
The Similarity Heatmap is useful to visualize the
blocks of similar code found in the genome, such as large tandem repeats, and isochores at all scales. ''')

#
# def reverseComplementOligs(oligCounts):
#     if hasDepth(oligCounts):  # this recurses until we're left with a single dictionary
#         return map(lambda x: reverseComplementOligs(x), oligCounts)
#
#     revCounts = {}
#     for key, value in oligCounts.iteritems():
#         revCounts[reverseComplement(key)] = value
#     return revCounts


def vectorizeCounts(oligCounts, orderedWords):
    if hasDepth(oligCounts):  # this recurses until we're left with a single dictionary
        return map(lambda x: vectorizeCounts(x, orderedWords), oligCounts)

    return [oligCounts[word] if word in oligCounts else 0 for word in orderedWords]


def calculateOutputPixels(state, heatMapState=SimilarityHeatMapState()):
    state.readFastaChunks()
    width = 300
    while len(state.seq) < (
            chunkSize * state.scale) + width * state.nucleotidesPerLine(): #all starting positions plus the maximum reach from the last line
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    oligCounts = OligomerUsage.countOligomers(state, heatMapState)  # count dict per line  [ {}, {}, {}, {}, {} ]
    orderedWords = generateExhaustiveOligomerList(heatMapState.oligomerSize)
    oligVector = vectorizeCounts(oligCounts, orderedWords)
    complementVector = vectorizeCounts(oligCounts, [reverseComplement(word) for word in orderedWords])

    heatMap = [[None for x in range(width)] for y in range(height)]

    for y in range(len(heatMap)):
        for x in range(0, len(heatMap[y])):
            if x == 0:
                heatMap[y][x] = 1.0  # don't bother calculating self:self
            elif x + y < len(oligVector):  # account for second to last chunk
                heatMap[y][x] = pearsonCorrelation(oligVector[y], complementVector[y + x])

                #TODO: the mirroring probably needs changing given the new heatmap png layout
                #    if heatMapState.useRowColumnCorrelation:
                #        mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
                ##        prettyPrint(heatMap)
                ##        print
                #        heatMap = rowColumnCorrelation(heatMap)
    median = 0.0 #medianFromAllLines(heatMap)

    pixels = twoSidedSpectrumColoring(heatMap, median)
    return pixels

