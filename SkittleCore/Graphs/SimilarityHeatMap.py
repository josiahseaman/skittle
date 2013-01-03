'''
Created on Dec 14, 2012

@author: Josiah
'''
from SkittleGraphTransforms import pearsonCorrelation, mirrorDiagonalMatrix, \
    rowColumnCorrelation
from SkittleStatePackets import StatePacket
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

def calculateOutputPixels(state, oligomerSize = 3, useRowColumnCorrelation = False):
    oligVectors = OligomerUsage.calculateOutputPixels(state, oligomerSize)
    heatMap = [[None for x in range(len(oligVectors))] for y in range(len(oligVectors))]
    
    for y in range(len(heatMap)):
        for x in range(y, len(heatMap[y])):
            if x == y:
                heatMap[y][x] = 1.0 #don't bother calculating self:self
            else:
                heatMap[y][x] = pearsonCorrelation(oligVectors[y], oligVectors[x])
    
    mirrorDiagonalMatrix(heatMap)#flip along diagonal symmetry
    if useRowColumnCorrelation:
        prettyPrint(heatMap)
        print
        heatMap = rowColumnCorrelation(heatMap)
    return heatMap


if __name__ == '__main__':
    state = StatePacket()
    state.width = 30
    oligomerSize = 2
    useRowColumnCorrelation = True
    heatMap = calculateOutputPixels(state, oligomerSize, useRowColumnCorrelation)

    prettyPrint(heatMap)

