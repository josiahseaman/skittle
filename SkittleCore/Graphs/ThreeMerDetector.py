'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from SkittleGraphTransforms import average
from PixelLogic import drawBar
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.models import RequestPacket
from SkittleGraphTransforms import sensitiveTestForSpecificFrequency, normalize, oldRepeatMap
from models import ThreeMerDetectorState
from MathLogic import lowPassFilter

registerGraph('t', "Threemer Detector", __name__, False, helpText='''Threemer detector was designed to detect the 
weak 3 periodicity signature associated with codons inside protein coding regions.  It is much more sensitive than 
Repeat Map, but only detects a single periodicity. Exon annotations are generally marked by a 3-mer spike. 
Strong 3-mer signals outside of exon annotation that are not simple repeats merit further research.''')

def testInformation(state, threemer_scores):
    threemer_scores.sort()
#    avg = average(threemer_scores)
#    median = threemer_scores[len(threemer_scores)/2]
#    percentile95 = threemer_scores[len(threemer_scores)*95/100]
#    max_ = threemer_scores[-1] 
    percentiles = []
    for p in range(50,100, 10):
        percentiles.append(threemer_scores[len(threemer_scores) * p /100])
    return (state.width, ) + percentiles



def calculateOutputPixels(state, threeMerState = ThreeMerDetectorState()):
    assert isinstance(state, RequestPacket)
    
    state.readFastaChunks()#read in next chunk
    scores = oldRepeatMap(state, threeMerState)
    
    threemer_scores = sensitiveTestForSpecificFrequency(scores, 3, threeMerState.samples)
    threemer_scores = lowPassFilter(threemer_scores)
    
#    return testInformation(state, threemer_scores)
    
    '''This trend was found experimentally based on maximums over 69 chunks at width 10-490 '''
    maximum = (2.4676524055 * state.nucleotidesPerLine() ** (-0.5070724543))*2#x2 low Pass Filter: this equation is based on the 95th Percentile of the low pass filtered data
    minimum = 1.0966679138 * state.nucleotidesPerLine() ** (-0.5441358102) #80th percentile

    outputPixels = []
    for size in threemer_scores:
        normalized = normalize(size, minimum, maximum)
        barSize = min(max(0, int(normalized * threeMerState.barWidth)), threeMerState.barWidth)
        barColor = (44, 85, 185)
        if normalized > 0.425:
            barColor = (93,4,157)
        bar = drawBar(barSize, int(threeMerState.barWidth- barSize), barColor, False)
        assert len(bar) == threeMerState.barWidth
        outputPixels.append( bar )
    return outputPixels
        
    