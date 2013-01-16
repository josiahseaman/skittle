'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import StatePacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph("Nucleotide Display", __name__, True)
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket) 
#    print state.seq
#    chunks = chunkUpList(state.seq, state.width )
#    print chunks
    if state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
#        print chunks
        counts = countNucleotides(chunks)
#        print counts
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
#        print pixels
    return pixels
    

    
    
    