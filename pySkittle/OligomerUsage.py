'''
Created on Dec 12, 2012

@author: Josiah
'''
from SkittleStatePackets import StatePacket
from SkittleGraphTransforms import chunkUpList, countNucleotides


def calculateOutputPixels(state, oligomerSize):
    assert isinstance(state, StatePacket)
    assert isinstance(oligomerSize, int)
    #chunk sequence by display line
    lines = chunkUpList(state.seq, state.width)
    counts = countNucleotides(lines, oligomerSize)#this is leaving off the ends of each line
    
    return counts



if __name__ == '__main__':
    state = StatePacket()
    oligomerSize = 3 
    print calculateOutputPixels(state, oligomerSize)