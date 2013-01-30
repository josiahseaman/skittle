'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('n',"Nucleotide Display", __name__, True, True)
    
def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket) 
#    print state.seq
#    chunks = chunkUpList(state.seq, state.nucleotidesPerLine() )
#    print chunks
    if state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
        print len(chunks)
        counts = countNucleotides(chunks)
        print len(counts)
        counts = normalizeDictionary(counts)
        print len(counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
        print len(pixels)
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
#        print pixels
    return pixels
    

    
    
    