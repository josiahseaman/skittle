'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from SkittleGraphTransforms import sensitiveTestForSpecificFrequency
from models import RepeatMapState
from SkittleCore.models import RequestPacket
import RepeatMap
from SkittleCore.GraphRequestHandler import registerGraph
from models import ThreeMerDetectorState
from PixelLogic import drawBar

registerGraph('t', "Threemer Detector", __name__, False)
    
def calculateOutputPixels(state, threeMerState = ThreeMerDetectorState()):
    assert isinstance(state, RequestPacket)
    state.scale = 1 #these calculations are only meaningful at scale 1
    #TODO adjust score scaling according to the width
    repeatMapState = RepeatMapState()
    repeatMapState.F_start = 1
    repeatMapState.F_width = threeMerState.samples * 3
    scores = RepeatMap.calculateOutputPixels(state, repeatMapState)
    threemer_scores = sensitiveTestForSpecificFrequency(scores, 3, threeMerState.samples)
    
    outputPixels = []
    for size in threemer_scores:
        barSize = int(size * threeMerState.barWidth)
        barColor = (255, 0, 255)
        if size > 0.5:
            barColor = (0,255,0)
        bar = drawBar(barSize, int(threeMerState.barWidth- barSize), barColor, False)
        outputPixels.append( bar )
    return outputPixels
    
    
    
    return threemer_scores

    
    