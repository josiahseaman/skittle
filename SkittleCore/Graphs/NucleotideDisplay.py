'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('n', "Nucleotide Display", __name__, True, True, helpText='''The four nucleotides of DNA are represented by four colors.
A=Black, T=Blue, C=Red, G=Green. The pixels are arranged across the screen like text, reading from left to right, 
then jumping to the beginning of the next line when it reaches the width. If width is set at a multiple of a tandem repeat, 
the repeat will appear as vertical bars.''')


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
    

    
    
    