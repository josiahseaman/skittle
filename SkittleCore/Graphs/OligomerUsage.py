'''
Created on Dec 12, 2012

@author: Josiah
'''
from SkittleCore.models import RequestPacket
from models import OligomerUsageState
from SkittleGraphTransforms import chunkUpList, countNucleotides, \
    normalizeDictionary, generateExhaustiveOligomerList, oligCountToColorSpace
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('o', "Oligomer Usage", __name__, False, isGrayScale=True, helpText='''Each row is one display line equal to width. 
Each column matches one oligomer of fixed size, arranged in alphabetical order (i.e. AA, AC, AG...). 
The brightness of the pixel indicates how often that oligomer occurred compared to all the others.''')


def calculateOutputPixels(state, oligState=OligomerUsageState()):
    assert isinstance(state, RequestPacket)
    assert isinstance(oligState, OligomerUsageState)
    counts = countOligomers(state, oligState)

    #NORMALIZATION
    values = []
    for line in counts:
        for key, value in line.iteritems():
            if key[0] != 'N':
                values.append(value)

    wholeScreenMaximum = max(values)
    counts = normalizeDictionary(counts, wholeScreenMaximum)

    #TODO: create a sparse display for the oligomer display
    orderedWords = generateExhaustiveOligomerList(oligState.oligomerSize)
    pixels = oligCountToColorSpace(counts, orderedWords)

    return pixels


def countOligomers(state, oligState=OligomerUsageState()):
    state.readFastaChunks()
    overlap = oligState.oligomerSize - 1
    # chunk sequence by display line #we can't do this simply by line because of the overhang of oligState.oligState
    lines = chunkUpList(state.seq, state.nucleotidesPerLine(), overlap)
    counts = countNucleotides(lines, oligState.oligomerSize)
    return counts

