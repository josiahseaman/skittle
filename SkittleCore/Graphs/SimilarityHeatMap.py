'''
Created on Dec 14, 2012

@author: Josiah
'''
from SkittleGraphTransforms import pearsonCorrelation, mirrorDiagonalMatrix, \
    rowColumnCorrelation
from SkittleCore.models import StatePacket
from models import SimilarityHeatMapState
import OligomerUsage
import SkittleRequestHandler

SkittleRequestHandler.registerGraph("Similarity Heatmap", __name__)

def prettyPrint(heatMap):
    for line in heatMap:
        print
        for e in line:
            if e:
                print round(e, 2), ', ',
            else: 
                print 'N', ', ',

def calculateOutputPixels(state, heatMapState):
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
        prettyPrint(heatMap)
        print
        heatMap = rowColumnCorrelation(heatMap)
    return heatMap


if __name__ == '__main__':
    state = StatePacket()
    state.width = 30
    heatMap = calculateOutputPixels(state, heatMapState = SimilarityHeatMapState())

    prettyPrint(heatMap)

