'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from SkittleGraphTransforms import sensitiveTestForSpecificFrequency
from models import RepeatMapState
from SkittleCore.models import StatePacket
import RepeatMap
import SkittleRequestHandler

SkittleRequestHandler.registerGraph("Threemer Detector", __name__)
    
def calculateOutputPixels(state, threeMerState):
    assert isinstance(state, StatePacket)
    state.scale = 1 #these calculations are only meaningful at scale 1
    #TODO adjust score scaling according to the width
    repeatMapState = RepeatMapState()
    repeatMapState.F_start = 1
    repeatMapState.F_width = threeMerState.samples * 3
    scores = RepeatMap.calculateOutputPixels(state, repeatMapState)
    threemer_scores = sensitiveTestForSpecificFrequency(scores, 3, threeMerState.samples)
    return threemer_scores

    
    