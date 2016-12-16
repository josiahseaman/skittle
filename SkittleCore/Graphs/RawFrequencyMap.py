'''
Created on March 2, 2013
@author: Josiah Seaman
'''
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.models import RequestPacket
from SkittleGraphTransforms import oldRepeatMap
from SkittleCore.PngConversionHelper import multiplyGreyscale
from models import ThreeMerDetectorState


registerGraph('f', "Raw Frequency Map", __name__, False, isGrayScale=True, helpText='''This graph is the raw data used to calculate
the Threemer Detector graph. The x-axis of Frequency Map represents offsets +1 to +60. The grey value of each pixel is the 
number of matching characters in the line compared with the sequence shifted to the offset.  Threemer patterns show up as faint
dark-dark-light dark-dark-light pixel patterns. These patterns are often associated with exons. ''')


def calculateOutputPixels(state, threeMerState=ThreeMerDetectorState()):
    assert isinstance(state, RequestPacket)

    state.readFastaChunks()#read in next chunk
    scores = oldRepeatMap(state, threeMerState)
    scores = multiplyGreyscale(scores)

    return scores
        
    