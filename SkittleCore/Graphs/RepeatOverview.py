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
    icolor = spectrum( progress )

    return interpolate((0,0,0), icolor ,0.0, 1.0, score ) #score should already be a floating point between 0.0 and 1.0

def convertRepeatDataToRepeatOverview(state, data):
    pixels = []
    if state.scale <= RepeatMap.skixelsPerSample:
        currentPosition = 0
        for lineNumber, line in enumerate(data):
            column, score = findMaxScore(line)
            while currentPosition < (lineNumber+1) * RepeatMap.skixelsPerSample: 
                pixels += [alignmentColor(score, column)]
                currentPosition += state.scale
    else:
        raise Exception("Scale > 24 " + str(state.scale))    
    return pixels 


def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket)

    data = RepeatMap.getBaseRepeatMapData(state)
    pixels = convertRepeatDataToRepeatOverview(state, data)
    return pixels
    