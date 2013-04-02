'''
Created on March 2, 2013
@author: Josiah Seaman
'''
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.models import RequestPacket
from SkittleGraphTransforms import oldRepeatMap
from models import ThreeMerDetectorState


registerGraph('f', "Raw Frequency Map", __name__, False)


def calculateOutputPixels(state, threeMerState = ThreeMerDetectorState()):
    assert isinstance(state, RequestPacket)
    state.scale = 1 #these calculations are only meaningful at scale 1
    
    state.readFastaChunks()#read in next chunk
    scores = oldRepeatMap(state, threeMerState)

    return scores
        
    