'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import NucleotideDisplay
from SkittleGraphTransforms import correlateColors 
from SkittleStatePackets import RepeatMapState
from SkittleStatePackets import StatePacket

'''
These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.
'''
def height(repeatMapState, pixels, width, scale):
    
    F_height = ((len(pixels)) - (repeatMapState.F_start-1)*scale - repeatMapState.F_width*scale ) / width
    F_height = max(0, min(400, F_height) )
    return F_height
    
def correlationMap(repeatMapState, coloredPixels, width, scale):
    pixelsPerSample = width / scale
    freq = []
    for h in range(height(repeatMapState, coloredPixels, width, scale)):
        freq.append([None]*(repeatMapState.F_width+1))
        offset = h * pixelsPerSample
        for w in range(1, len(freq[h])):#calculate across widths 1:F_width
            coefficient = correlateColors(coloredPixels, offset, offset + w + repeatMapState.F_start, pixelsPerSample)
            if coefficient != None:
                freq[h][w] = .5 * (1.0 + coefficient)
    return freq
  

def calculateOutputPixels(state, repeatMapState):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, StatePacket)
    
    pixels = NucleotideDisplay.calculateOutputPixels(state)
    scores = correlationMap(repeatMapState, pixels, state.width, state.scale)
    return scores
    
        
if __name__ == '__main__':
    print 'Repeat Map test case'
    state = StatePacket()
    repeatMapState = RepeatMapState()
    print calculateOutputPixels(state, repeatMapState)
    
    