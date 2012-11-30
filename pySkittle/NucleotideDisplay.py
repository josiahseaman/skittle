'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import sequenceToColors
from SkittleGraphTransforms import colorCompress
from SkittleStatePackets import StatePacket
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket) 
    seq = state.seq[state.start-1 : (state.start-1) + state.length] #substrings the relevant part
    pixels = sequenceToColors(seq) 
    if state.scale > 1:
        pixels = colorCompress(pixels, state.scale)
    return pixels
    
if __name__ == '__main__':
    print 'Nucleotide Display test case'
    state = StatePacket()
    print calculateOutputPixels(state)

    
    
    