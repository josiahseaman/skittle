"""
Created on January 3, 2017
@author: Josiah
"""
from collections import defaultdict

import math

from SkittleCore.Graphs.ReverseComplementMap import setOfObservedOligs, reverseComplementSet
from SkittleCore.Graphs.SkittleGraphTransforms import normalize, reverseComplement
from models import ReverseComplementState
from SkittleCore.models import chunkSize
from SkittleCore.GraphRequestHandler import registerGraph


registerGraph('v', "Tag Vision", __name__, True, False, isGrayScale=True,
              helpText='''This graph is a raster version of Reverse Complement Map.
This shows where reverse complement tags are in the sequence, but not what they link to.''')


def calculateOutputPixels(state, stateDetail=ReverseComplementState()):
    state.readFastaChunks()
    width = 300  # number of line downstream that will be compared.  The last line only shows up a the corner of the screen
    tag_candidates = defaultdict(lambda: 0)
    while len(state.seq) < (  # all starting positions plus the maximum reach from the last line
            chunkSize * state.scale) + width * state.nucleotidesPerLine():
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize * state.scale) / float(state.nucleotidesPerLine())))

    observedOligsPerLine = setOfObservedOligs(state.seq, state.nucleotidesPerLine(), stateDetail.oligomerSize)
    observedRevCompOligs = reverseComplementSet(observedOligsPerLine)
    # observedRevCompOligs = reversed(setOfObservedOligs(reverseComplement(state.seq), state.nucleotidesPerLine(), stateDetail.oligomerSize))

    for y in range(min(len(observedOligsPerLine), height)):
        for x in range(0, min(len(observedOligsPerLine) - y - 1, width)):
            if not x == 0 and x + y < len(observedOligsPerLine):  # account for second to last chunk
                matches = observedOligsPerLine[y].intersection(observedRevCompOligs[y + x])
                otherStrand = {reverseComplement(word) for word in matches}
                matches.update(otherStrand)  # merge with its own Reverse Complement
                if matches:
                    for olig in matches:
                        tag_candidates[olig] += 1

    # with open('_'.join(['candidates_olig%i' % stateDetail.oligomerSize, str(state.chunkStart()), str(state.nucleotidesPerLine())]) + '.csv', 'w') as tag_file:
    #     tag_file.write('\n'.join(["%s,%i" % (o, c) for o, c in tag_candidates.items()]))

    olig_size = stateDetail.oligomerSize
    hit_list = [0] * chunkSize
    for begin in range(len(hit_list) - olig_size):
        olig = state.seq[begin: begin + olig_size]
        if olig in tag_candidates:
            for pos in range(begin, begin + olig_size):
                hit_list[pos] = max(hit_list[pos], tag_candidates[olig])

    m = max(hit_list)
    pixels = [int(normalize(i, 0, m) * 255) for i in hit_list]

    return pixels