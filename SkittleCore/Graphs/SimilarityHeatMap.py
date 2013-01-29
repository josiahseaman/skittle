'''
Created on Dec 14, 2012

@author: Josiah
'''
from SkittleGraphTransforms import pearsonCorrelation, mirrorDiagonalMatrix, \
    rowColumnCorrelation
from SkittleCore.models import RequestPacket, chunkSize
from models import SimilarityHeatMapState
import OligomerUsage
from SkittleCore.GraphRequestHandler import registerGraph
from PixelLogic import twoSidedSpectrumColoring
import copy
import math

registerGraph('s', "Similarity Heatmap", __name__, False)

def prettyPrint(heatMap):
    for line in heatMap:
        print
        for e in line:
            if isinstance(e, float):
                print round(e, 2), ', ',
            else: 
                print e, ', ',
    print #newline

def calculateOutputPixels(state, heatMapState = SimilarityHeatMapState()):
    while len(state.seq) < 65536 + 300 * state.width:
        state.readAndAppendNextChunk(True)
    width = 300
    height = int(math.ceil(chunkSize / float(state.width))) 
    
    oligVectors = OligomerUsage.calculateOutputPixels(state, heatMapState)#TODO: performance: cap at 300 lines past the chunk boundary
    heatMap = [[None for x in range(width)] for y in range(height)]
    
    for y in range(len(heatMap)):
        for x in range(0, len(heatMap[y])):
            if x == 0:
                heatMap[y][x] = 1.0 #don't bother calculating self:self
            elif x+y < len(oligVectors):#account for second to last chunk
                heatMap[y][x] = pearsonCorrelation(oligVectors[y], oligVectors[ y+x ] )
                
    if heatMapState.useRowColumnCorrelation:
        mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
#        prettyPrint(heatMap)
#        print
        heatMap = rowColumnCorrelation(heatMap)
        
    pixels = twoSidedSpectrumColoring(heatMap)
    return pixels

