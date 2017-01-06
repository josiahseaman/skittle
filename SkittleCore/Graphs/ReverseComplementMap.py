'''
Created on March 11, 2016

@author: Josiah
'''
import math
from collections import defaultdict

from SkittleGraphTransforms import reverseComplement, hasDepth, chunkUpList
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


def setOfObservedOligs(seq, lineSize, oligomerSize):
    # TODO: refactor and remove duplication from OligomerUsage.countOligomers()
    """
    :return: list of set of strings
    :rtype: list
    """
    overlap = oligomerSize - 1
    # chunk sequence by display line #we can't do this simply by line because of the overhang of oligState.oligState
    lines = chunkUpList(seq, lineSize, overlap)

    oligsByLine = []
    for line in lines:
        oligsByLine.append(observedOligs(line, oligomerSize))

    return oligsByLine


def matches_on_one_line(state, secondStart, oligomerSize):
    state.readFastaChunks()
    if secondStart - state.chunkStart() > chunkSize:
        state.readAndAppendNextChunk()
    matches = olig_matches_core(state, secondStart, oligomerSize)
    adv = set()
    if matches:
        for x in range(oligomerSize + 1, oligomerSize + 1):
            temp = matches_on_one_line(state, secondStart, x)
            if temp:
                adv = temp
            else:
                break
    return list(adv) + list(matches)


def olig_matches_core(state, secondStart, oligomerSize):
    stateDetail = ReverseComplementState()
    stateDetail.oligomerSize = oligomerSize
    sample_size = state.nucleotidesPerLine() + stateDetail.oligomerSize - 1
    secondStart -= state.chunkStart()
    seq_A = state.seq[state.relativeStart(): state.relativeStart() + sample_size]
    oligs_A = observedOligs(seq_A, stateDetail.oligomerSize)
    seq_B = state.seq[secondStart: secondStart + sample_size]
    oligs_B = reverseComplementSet([observedOligs(seq_B, stateDetail.oligomerSize)])[0]
    matches = oligs_A.intersection(oligs_B)
    return matches


def calculateOutputPixels(state, stateDetail=ReverseComplementState()):
    state.readFastaChunks()
    width = 300  # number of line downstream that will be compared.  The last line only shows up a the corner of the screen
    observedMax = max(state.nucleotidesPerLine() / 6.0, 5.0)  # defined by observation of histograms
    tag_candidates = defaultdict(lambda: 0)
    while len(state.seq) < (  # all starting positions plus the maximum reach from the last line
            chunkSize * state.scale) + width * state.nucleotidesPerLine():
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    observedOligsPerLine = setOfObservedOligs(state.seq, state.nucleotidesPerLine(), stateDetail.oligomerSize)
    observedRevCompOligs = reverseComplementSet(observedOligsPerLine)
    heatMap = [[0 for x in range(width)] for y in range(height)]

    for y in range(min(len(observedOligsPerLine), len(heatMap))):
        for x in range(0, min(len(observedOligsPerLine) - y - 1, width)):
            if x == 0:
                heatMap[y][x] = int(0.25 * 255)  # don't bother calculating self:self
            elif x + y < len(observedOligsPerLine):  # account for second to last chunk
                matches = observedOligsPerLine[y].intersection(observedRevCompOligs[y + x])
                if matches:
                    heatMap[y][x] = int(min(len(matches) / observedMax * 255, 255))
                    matches.update({reverseComplement(word) for word in matches})  # merge with its own Reverse Complement
                    for olig in matches:
                        tag_candidates[olig] += 1

    # with open('candidates_olig%i_start%i_width%i.csv' % (stateDetail.oligomerSize, state.chunkStart(), state.nucleotidesPerLine()), 'w') as tag_file:
    #     tag_file.write('\n'.join(["%s,%i" % (o, c) for o, c in tag_candidates.items()]))

    return heatMap
