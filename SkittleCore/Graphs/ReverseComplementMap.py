'''
Created on March 11, 2016

@author: Josiah
'''
import math

from django.conf import settings

from SkittleGraphTransforms import reverseComplement, hasDepth, chunkUpList, normalize
from SkittleCore.models import chunkSize
from models import ReverseComplementState
from SkittleCore.GraphRequestHandler import registerGraph


registerGraph('c', "Reverse Complement Map", __name__, False, False, 0.0, stretchy=False, isGrayScale=True,
              helpText='''This graph is a heatmap that shows nearby reverse complementary sequences.
Look for short diagonal lines perpendicular to the main axis (upper left to bottom right).
The structure of a heatmap is diagonally symmetrical.
The diagonal grey line is self compared with self.
Each pixel represents a comparison between two other lines in the Nucleotide Display.
The crosshairs show you which lines are involved in a comparison.''')


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


def observedOligs(seq, oligomerSize):
    oligs = set()
    for endIndex in range(oligomerSize, len(seq) + 1, 1):
        window = seq[endIndex - oligomerSize: endIndex]
        oligs.add(window)
    return oligs


def setOfObservedOligs(state, oligomerSize):
    # TODO: refactor and remove duplication from OligomerUsage.countOligomers()
    """
    :return: list of set of strings
    :rtype: list
    """
    overlap = oligomerSize - 1
    # chunk sequence by display line #we can't do this simply by line because of the overhang of oligState.oligState
    lines = chunkUpList(state.seq, state.nucleotidesPerLine(), overlap)

    oligsByLine = []
    for seq in lines:
        oligsByLine.append(observedOligs(seq, oligomerSize))

    return oligsByLine


def matches_on_one_line(state, secondStart, oligomerSize, stateDetail=ReverseComplementState()):
    stateDetail.oligomerSize = oligomerSize
    state.readFastaChunks()  # TODO: something more efficient for one line
    sample_size = state.nucleotidesPerLine() + stateDetail.oligomerSize - 1
    secondStart -= state.chunkStart()
    seq_A = state.seq[state.relativeStart(): state.relativeStart() + sample_size]
    oligs_A = observedOligs(seq_A, stateDetail.oligomerSize)
    seq_B = state.seq[secondStart: secondStart + sample_size]
    oligs_B = reverseComplementSet([observedOligs(seq_B, stateDetail.oligomerSize)])[0]
    matches = oligs_A.intersection(oligs_B)
    return list(matches)


def calculateOutputPixels(state, stateDetail=ReverseComplementState()):
    state.readFastaChunks()
    width = 300  # number of line downstream that will be compared.  The last line only shows up a the corner of the screen
    maxLength = 6500 * 2  # debugging variable
    observedMax = max(state.nucleotidesPerLine() / 6.0, 5.0)  # defined by observation of histograms
    # if settings.DEBUG:
    #     state.seq = state.seq[:maxLength]
    # else:
    while len(state.seq) < (  # all starting positions plus the maximum reach from the last line
            chunkSize * state.scale) + width * state.nucleotidesPerLine():
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    observedOligsPerLine = setOfObservedOligs(state, stateDetail.oligomerSize)
    observedRevCompOligs = reverseComplementSet(observedOligsPerLine)
    heatMap = [[0.0 for x in range(width)] for y in range(height)]
    # histogram = [0 for x in range(state.nucleotidesPerLine())]

    for y in range(min(len(observedOligsPerLine), len(heatMap))):
        for x in range(0, min(len(observedOligsPerLine) - y - 1, width)):
            if x == 0:
                heatMap[y][x] = 0.25  # don't bother calculating self:self
            elif x + y < len(observedOligsPerLine):  # account for second to last chunk
                matches = len(observedOligsPerLine[y].intersection(observedRevCompOligs[y + x]))
                if matches:
                    heatMap[y][x] = matches / observedMax
                # observedMax = max(observedMax, matches)
                # histogram[matches] += 1

    # observedMax = observedMax / 2.0  # math.log(observedMax)
    # for y in range(len(heatMap)):
    #     for x in range(0, len(heatMap[y])):
    #         if x != 0 and heatMap[y][x]:
    #             heatMap[y][x] = heatMap[y][x] / observedMax  # normalization  math.log(

    # with open('_'.join(['hist_olig9', str(state.chunkStart()), str(state.nucleotidesPerLine())]) + '.csv', 'w') as his_file:
    #     for i, count in enumerate(histogram):
    #         his_file.write('%i,%i\n' % (i, count))

    return heatMap
