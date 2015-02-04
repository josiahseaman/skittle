'''
Created on Feb, 2015
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('d', "RepeatMap Detail", __name__, True, True, helpText='''The exact sequence pair comparison that led to
each pixel of the RepeatMap is shown.''')


def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)
    #    chunks = chunkUpList(state.seq, state.nucleotidesPerLine() )

    if state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
        counts = countNucleotides(chunks)
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette, state.scale)
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
    return pixels
    

    
    
    