'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import StatePacket
from SkittleCore.SkittleRequestHandler import registerGraph

registerGraph("Nucleotide Display", __name__)
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket) 
    print state.seq 
    chunks = chunkUpList(state.seq, state.width )
    print chunks
    if state.scale > 1:
        chunks = chunkUpList(chunks, state.scale)
        print chunks
        counts = countNucleotides(chunks)
        print counts
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
    else:
        pixels = sequenceToColors(chunks, state.colorPalette)
    return pixels
    

    
    
    