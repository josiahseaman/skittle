'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import NucleotideDisplay
from SkittleGraphTransforms import correlationMap
from SkittleStatePackets import RepeatMapState
from SkittleStatePackets import StatePacket

'''
These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.
'''


def calculateOutputPixels(state, repeatMapState):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, StatePacket)
    
    pixels = NucleotideDisplay.calculateOutputPixels(state)
    scores = correlationMap(state, repeatMapState, pixels) #2D array
    #TODO convert from floating point to grey pixels
    return scores
    
        
if __name__ == '__main__':
    print 'Repeat Map test case'
    state = StatePacket()
    repeatMapState = RepeatMapState()
    print calculateOutputPixels(state, repeatMapState)
    
    