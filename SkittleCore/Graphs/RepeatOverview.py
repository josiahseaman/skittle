'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import RepeatMap
from SkittleGraphTransforms import normalize
from PixelLogic import interpolate, spectrum
from models import RepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph
from DNAStorage.StorageRequestHandler import GetPngFilePath
from SkittleCore.png import Reader
from SkittleCore.Graphs.MathLogic import ceil, floor
import copy

registerGraph('r', "Repeat Overview", __name__, True, False)

def findMaxScore(line):
    valueToBeat = 0.0
    column = 0
    score = 0.0
    for index, value in enumerate(line):
        if value > valueToBeat:
            column, score = index, value
            valueToBeat = score * 1.05 #this is made to bias the "winner" towards lower number repeats, rather than a multiple of the base frequency
    return column, score

def alignmentColor(score, column):
    defaultSettings = RepeatMapState()
    progress = normalize(column, 1, defaultSettings.numberOfColumns())
    icolor = spectrum( progress*2 )

    return interpolate((0,0,0), icolor ,0.0, 1.0, max(0.0, score-.5)*3.0 ) #score should already be a floating point between 0.0 and 1.0

def convertRepeatDataToRepeatOverview(state, data, nucleotidesPerLine):
    #TODO: generic phased sample method
    pixels = []
    currentPosition = 0
    for lineNumber, line in enumerate(data):
        column, score = findMaxScore(line)
        while currentPosition < (lineNumber+1) * nucleotidesPerLine: 
            pixels += [alignmentColor(score, column)]
            currentPosition += state.scale
    while len(pixels) < chunkSize:
        pixels += pixels[-1:]    
    return pixels 


def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket)
    
    targetScale = ceil(float(state.scale) / RepeatMap.skixelsPerSample )
    if targetScale == 1:
        data = RepeatMap.getBaseRepeatMapData(state)
    else:
        tempState = copy.deepcopy(state)
#        targetPerLine = 
        tempState.width = 1
        data = RepeatMap.squishStoredMaps(tempState)
    
    pixels = convertRepeatDataToRepeatOverview(state, data, RepeatMap.skixelsPerSample * targetScale)
    return pixels
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    