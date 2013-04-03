'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('n',"Nucleotide Display", __name__, True, True)
    
def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket) 
#    chunks = chunkUpList(state.seq, state.nucleotidesPerLine() )

    if state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
        counts = countNucleotides(chunks)
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
    return pixels
    

    
    
    