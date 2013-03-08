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

registerGraph('s', "Similarity Heatmap", __name__, False, False, 0.4)

def prettyPrint(heatMap):
    for line in heatMap:
        print
        for e in line:
            if isinstance(e, float):
                print round(e, 2), ', ',
            else: 
                print e, ', ',
    print #newline

def medianFromAllLines(heatMap):
    normalizationList = []
    for line in heatMap:
        normalizationList += line
    normalizationList.sort()
    return normalizationList[ len(normalizationList)/2]
    

def calculateOutputPixels(state, heatMapState = SimilarityHeatMapState()):
    state.readFastaChunks()
    width = 300
    while len(state.seq) < (chunkSize*state.scale) + width * state.nucleotidesPerLine(): #all starting positions plus the maximum reach from the last line
        state.readAndAppendNextChunk(True)
    height = int(math.ceil((chunkSize*state.scale) / float(state.nucleotidesPerLine()))) 
    
    oligVectors = OligomerUsage.calculateOutputPixels(state, heatMapState)#TODO: performance: cap at 300 lines past the last chunk boundary
    heatMap = [[None for x in range(width)] for y in range(height)]
    
    for y in range(len(heatMap)):
        for x in range(0, len(heatMap[y])):
            if x == 0:
                heatMap[y][x] = 1.0 #don't bother calculating self:self
            elif x+y < len(oligVectors):#account for second to last chunk
                heatMap[y][x] = pearsonCorrelation(oligVectors[y], oligVectors[ y+x ] )
                
#TODO: the mirroring probably needs changing given the new heatmap png layout
#    if heatMapState.useRowColumnCorrelation: 
#        mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
##        prettyPrint(heatMap)
##        print
#        heatMap = rowColumnCorrelation(heatMap)
    median = 0.0 #medianFromAllLines(heatMap)

    pixels = twoSidedSpectrumColoring(heatMap, median)
    return pixels

