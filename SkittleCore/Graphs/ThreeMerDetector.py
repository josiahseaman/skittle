'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from SkittleGraphTransforms import sensitiveTestForSpecificFrequency
from SkittleStatePackets import RepeatMapState
from SkittleStatePackets import StatePacket
import RepeatMap
import SkittleRequestHandler

SkittleRequestHandler.registerGraph("Threemer Detector", __name__)
    
def calculateOutputPixels(state):
    assert isinstance(state, StatePacket)
    barWidth = 20    #used for display size calculations
    samples = 20
    state.scale = 1 #these calculations are only meaningful at scale 1
    #TODO adjust score scaling according to the width
    repeatMapState = RepeatMapState()
    repeatMapState.F_start = 1
    repeatMapState.F_width = samples * 3
    scores = RepeatMap.calculateOutputPixels(state, repeatMapState)
    threemer_scores = sensitiveTestForSpecificFrequency(scores, 3, samples)
    return threemer_scores

        
if __name__ == '__main__':
    print '3mer Detector test case'
    state = StatePacket()
    print calculateOutputPixels(state)
    
    