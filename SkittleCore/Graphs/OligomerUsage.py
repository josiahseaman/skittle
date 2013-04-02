'''
Created on Dec 12, 2012

@author: Josiah
'''
from SkittleCore.models import RequestPacket
from models import OligomerUsageState
from SkittleGraphTransforms import chunkUpList, countNucleotides,\
    normalizeDictionary, generateExhaustiveOligomerList, oligCountToColorSpace
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('o', "Oligomer Usage", __name__, False, isGrayScale=True)

def calculateOutputPixels(state, oligState = OligomerUsageState()):
    assert isinstance(state, RequestPacket)
    assert isinstance(oligState, OligomerUsageState)
    state.readFastaChunks()

    overlap = oligState.oligomerSize-1
    lines = chunkUpList(state.seq, state.nucleotidesPerLine(), overlap) #chunk sequence by display line #we can't do this simply by line because of the overhang of oligState.oligState

    counts = countNucleotides(lines, oligState.oligomerSize)
    
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

