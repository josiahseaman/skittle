'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from SkittleGraphTransforms import average
from PixelLogic import drawBar
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.models import RequestPacket
from SkittleGraphTransforms import sensitiveTestForSpecificFrequency, normalize
from models import RepeatMapState, ThreeMerDetectorState
import RepeatMap
import math

registerGraph('t', "Threemer Detector", __name__, False)



def countMatches(sequence, beginA, beginB, lineSize):
    matches = 0
    for index in range(lineSize):
        if sequence[beginA + index] == sequence[beginB + index]:
            matches += 1
    return float(matches) / lineSize

def oldRepeatMap(state, threeMerState):
    freq = []
    lineSize = state.nucleotidesPerLine()
    for h in range(threeMerState.height(state, state.seq)):
        freq.append([0.0]*(threeMerState.samples*3+1))
        offset = h * lineSize
        for w in range(1, len(freq[h])):
            freq[h][w] = countMatches(state.seq, offset, offset + w + 1, lineSize)
    return freq

def calculateOutputPixels(state, threeMerState = ThreeMerDetectorState()):
    assert isinstance(state, RequestPacket)
    state.scale = 1 #these calculations are only meaningful at scale 1
    
#    if RepeatMap.checkForCachedMap(state):
#        scores = RepeatMap.squishStoredMaps(state)
#        threeMerState.samples = 8 #there's less to work with in the cached version
#    else:
    state.readFastaChunks()#read in next chunk
    scores = oldRepeatMap(state, threeMerState)
    
    threemer_scores = sensitiveTestForSpecificFrequency(scores, 3, threeMerState.samples)
    
    '''This trend was found experimentally based on maximums over 69 chunks at width 10-490  #max(threemer_scores)'''
    maximum = min(0.2, 1.725816397 * (state.width / 69.0 * 20.0)**(-0.6403354918)) / 4.0
    minimum = 0  #min(threemer_scores)    

    outputPixels = []
    for size in threemer_scores:
        normalized = normalize(size, minimum, maximum)
        barSize = min(max(0, int(normalized * threeMerState.barWidth)), threeMerState.barWidth)
        barColor = (44, 85, 185)
        if normalized > 0.75:
            barColor = (93,4,157)
        bar = drawBar(barSize, int(threeMerState.barWidth- barSize), barColor, False)
        assert len(bar) == threeMerState.barWidth
        outputPixels.append( bar )
    return outputPixels
        
    