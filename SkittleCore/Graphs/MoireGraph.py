'''
Created on Apr 9, 2013
@author: Josiah
'''

from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph
import NucleotideDisplay

registerGraph('x',"Moire Graph", __name__, True, True)
    
def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket)
    state.readFastaChunks() 
    
    sampling = [state.seq[i] for i in range(0, len(state.seq), state.scale)]
    #state.seq = ''.join(sampling)
    temp = state.copy()
    temp.seq = ''.join(sampling)
    temp.scale = 1
    pixels = NucleotideDisplay.calculateOutputPixels(temp)
    return pixels
    
    