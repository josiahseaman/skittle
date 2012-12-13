'''
Created on Dec 12, 2012

@author: Josiah
'''
from SkittleStatePackets import StatePacket
from SkittleGraphTransforms import chunkUpList, countNucleotides,\
    normalizeDictionary, generateExhaustiveOligomerList, oligCountToColorSpace


def calculateOutputPixels(state, oligomerSize):
    assert isinstance(state, StatePacket)
    assert isinstance(oligomerSize, int)
    #chunk sequence by display line
    print state.seq
#    lines = chunkUpList(state.seq, state.width)
    overlap = oligomerSize-1
    lines = chunkUpList(state.seq, state.width, overlap)#we can't do this simply by line because of the overhang of oligomerSize
    print lines
    counts = countNucleotides(lines, oligomerSize)
    counts = normalizeDictionary(counts, max)#this is currently per line normalization.  Test to see which is more/less confusing
    print counts
    #per line normalization is going to screw up the math used in the similarity heat map, make sure not to use normalized data for that
    #TODO create a sparse display for the oligomer display
    orderedWords = generateExhaustiveOligomerList(oligomerSize)
    pixels = oligCountToColorSpace(counts, orderedWords)
    
    return pixels



if __name__ == '__main__':
    state = StatePacket()
    oligomerSize = 2
    print calculateOutputPixels(state, oligomerSize)