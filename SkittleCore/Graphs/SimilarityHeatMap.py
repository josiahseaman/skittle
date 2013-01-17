'''
Created on Dec 14, 2012

@author: Josiah
'''
from SkittleGraphTransforms import pearsonCorrelation, mirrorDiagonalMatrix, \
    rowColumnCorrelation
from SkittleCore.models import StatePacket
from models import SimilarityHeatMapState
import OligomerUsage
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('s', "Similarity Heatmap", __name__, False)

def prettyPrint(heatMap):
    for line in heatMap:
        print
        for e in line:
            if e:
                print round(e, 2), ', ',
            else: 
                print 'N', ', ',
    print #newline

def calculateOutputPixels(state, heatMapState = SimilarityHeatMapState()):
    oligVectors = OligomerUsage.calculateOutputPixels(state, heatMapState)
    heatMap = [[None for x in range(len(oligVectors))] for y in range(len(oligVectors))]
    
    for y in range(len(heatMap)):
        for x in range(y, len(heatMap[y])):
            if x == y:
                heatMap[y][x] = 1.0 #don't bother calculating self:self
            else:
                heatMap[y][x] = pearsonCorrelation(oligVectors[y], oligVectors[x])
    
    mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
    if heatMapState.useRowColumnCorrelation:
#        prettyPrint(heatMap)
#        print
        heatMap = rowColumnCorrelation(heatMap)
    return heatMap

