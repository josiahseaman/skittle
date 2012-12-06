'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import countNucleotideGroups, normalizeDictionary, countListToColorSpace, sequenceToColors
from SkittleStatePackets import StatePacket
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket) 
    seq = state.seq[state.start : state.start + state.length] #substrings the relevant part 
    if state.scale > 1:
        counts = countNucleotideGroups(seq, state.scale)
        counts = map(lambda group: normalizeDictionary(group), counts)
        pixels = countListToColorSpace(counts, state.colorPalette)
    else:        
        pixels = sequenceToColors(seq, state.colorPalette)
    return pixels
    
if __name__ == '__main__':
    print 'Nucleotide Display test case'
    state = StatePacket()
    print calculateOutputPixels(state)

    
    
    