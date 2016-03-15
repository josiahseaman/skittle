'''
Created on March 11, 2016

@author: Josiah
'''
import math

from SkittleGraphTransforms import reverseComplement, hasDepth, chunkUpList
from SkittleCore.models import chunkSize
from models import SimilarityHeatMapState
from SkittleCore.GraphRequestHandler import registerGraph


registerGraph('c', "Reverse Complement Map", __name__, False, False, 0.4, isGrayScale=True, helpText='''This graph is a heatmap that shows
nearby reverse complementary sequences.  Look for short diagonal lines perpendicular to the main axis (upper left to
bottom right).
The structure of a heatmap is diagonally symmetrical.
The diagonal red line is self compared with self.
Each pixel represents a comparison between two other lines.
To see which lines are involved in a comparison trace one line straight down to the diagonal and another line to the left.
The Similarity Heatmap is useful to visualize the
blocks of similar code found in the genome, such as large tandem repeats, and isochores at all scales. ''')


def vectorizeCounts(oligCounts, orderedWords):
    if hasDepth(oligCounts):  # this recurses until we're left with a single dictionary
        return map(lambda x: vectorizeCounts(x, orderedWords), oligCounts)

    return [oligCounts[word] if word in oligCounts else 0 for word in orderedWords]


def reverseComplementSet(observedOligsPerLine):
    """
    :param observedOligsPerLine:
    :rtype: list of set of strings
    :return: set of reverse complements of input set
    """
    lines = []
    for oligs in observedOligsPerLine:
        lines.append({reverseComplement(word) for word in oligs})
    return lines


def setOfObservedOligs(state, oligomerSize):
    # TODO: refactor and remove duplication from OligomerUsage.countOligomers()
    """
    :return: list of set of strings
    :rtype: list
    """
    state.readFastaChunks()
    overlap = oligomerSize - 1
    # chunk sequence by display line #we can't do this simply by line because of the overhang of oligState.oligState
    lines = chunkUpList(state.seq, state.nucleotidesPerLine(), overlap)

    oligsByLine = []
    for seq in lines:
        oligs = set()
        for endIndex in range(oligomerSize, len(seq) + 1, 1):
            window = seq[endIndex - oligomerSize: endIndex]
            oligs.add(window)
        oligsByLine.append(oligs)

    return oligsByLine


def calculateOutputPixels(state, heatMapState=SimilarityHeatMapState()):
    heatMapState.oligomerSize = 9
    state.readFastaChunks()
    width = 300
    expectedMax = state.nucleotidesPerLine() / float(heatMapState.oligomerSize)
    while len(state.seq) < (
            chunkSize * state.scale) + width * state.nucleotidesPerLine(): #all starting positions plus the maximum reach from the last line
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    observedOligsPerLine = setOfObservedOligs(state, heatMapState.oligomerSize)
    observedRevCompOligs = reverseComplementSet(observedOligsPerLine)
    heatMap = [[None for x in range(width)] for y in range(height)]

    for y in range(len(heatMap)):
        for x in range(0, len(heatMap[y])):
            if x == 0:
                heatMap[y][x] = 0.25  # don't bother calculating self:self
            elif x + y < len(observedOligsPerLine):  # account for second to last chunk
                heatMap[y][x] = len(observedOligsPerLine[y].intersection(observedRevCompOligs[y + x])) / expectedMax  # normalization

    return heatMap
