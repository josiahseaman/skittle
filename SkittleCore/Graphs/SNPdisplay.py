'''
Created on Feb 11, 2013
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph
import SNPdata

registerGraph('p',"SNP Display", __name__, True, True)
    
def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket) 
    seq = SNPdataToSequence(SNPdata.SNPdata,state.start)
    pixels = sequenceToColors(seq, state.colorPalette)
    if state.scale > 1:
        pixels = sparceAverage(pixels,state.scale)

    return pixels
    
def sparceAverage(pixels,scale):
    return pixels
    
def SNPdataToSequence(SNPdataData,start):
    seq = ["N"] * chunkSize
    for SNP in SNPdataData:
        if SNP[3] - start < chunkSize and SNP[3] - start > 0:
            seq[SNP[3] - start] = SNPdata.packedSNPs[SNP[0]*2]
            seq[SNP[3]+1 - start] = SNPdata.packedSNPs[SNP[0]*2+1]
    return ''.join(seq)
