"""
Created on January 23, 2017

@author: Josiah
"""

from __future__ import print_function
import math
from math import ceil
from collections import defaultdict

from SkittleCore.Graphs.ReverseComplementMap import setOfObservedOligs, reverseComplementSet
from SkittleCore.Graphs.SkittleGraphTransforms import reverseComplement
from SkittleCore.models import chunkSize
from models import ReverseComplementState
from SkittleCore.GraphRequestHandler import registerGraph
from PixelLogic import twoSidedSpectrumColoring


registerGraph('p', "Tag Pair Map", __name__, False, False, 0.4, helpText='''Each line on this diagonally symmetrical graph
shows a pair of Reverse Complement oligomers. Scoring is based on how often the pair show up together on this chromosome.
Clusters of pairs are scored higher than a single one and can chain together into longer oligomers.

Tag candidates are computed across a whole chromosome then highlighted locally.''')


def cutoff_count(seq_size, olig_size):
    return max(ceil(seq_size / (4**olig_size)), 1)
# cutoff_count(4000000, 9)
# Out[]: 16


def readTagCandidates(state, stateDetail):
    # check if tag candidates are already computed
    #    Read and return
    # compute them for whole chromosome
    # Save to file
    # Return
    pass


def calculateOutputPixels(state, stateDetail=ReverseComplementState()):
    readTagCandidates(state, stateDetail)

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
    pixels = twoSidedSpectrumColoring(heatMap, median)
    return pixels

