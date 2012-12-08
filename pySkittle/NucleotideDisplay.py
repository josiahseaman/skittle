'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleStatePackets import StatePacket
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket) 
    seq = state.seq[state.start : state.start + state.length] #substrings the relevant part
    print seq 
    if state.scale > 1:
        chunks = chunkUpList(seq, state.width )
        print chunks
        chunks = chunkUpList(chunks, state.scale)
        print chunks
        counts = countNucleotides(chunks)
        print counts
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
    else:        
        pixels = sequenceToColors(seq, state.colorPalette)
    return pixels
    
if __name__ == '__main__':
    print 'Nucleotide Display test case'
    state = StatePacket()
    print calculateOutputPixels(state)

    
    
    