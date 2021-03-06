'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import copy

import RepeatMap
from SkittleGraphTransforms import normalize
from PixelLogic import interpolate, spectrum
from models import RepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.Graphs.MathLogic import ceil


registerGraph('r', "Repeat Overview", __name__, True, False, helpText='''Repeat Overview is a color based overview of the Repeat Map.
Each pixel in the overview is built from one full line of Repeat Map at width 24.  It looks for the maximum score anywhere on a Repeat Map line.
The brightness of the pixel represents the highest score.  Dark areas of the overview mean there are no repeats.  The color in bright areas
represents the length (frequency) of a tandem repeat that is detected.''')


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
    icolor = spectrum(progress * 1.5)

    return interpolate((0, 0, 0), icolor, 0.0, 1.0,
                       max(0.0, score - .5) * 3.0) #score should already be a floating point between 0.0 and 1.0


def convertRepeatDataToRepeatOverview(state, data, nucleotidesPerLine):
    #TODO: generic phased sample method
    pixels = []
    currentPosition = 0
    for lineNumber, line in enumerate(data):
        column, score = findMaxScore(line)
        while currentPosition < (lineNumber + 1) * (nucleotidesPerLine):
            pixels += [alignmentColor(score, column)]
            currentPosition += state.scale
    while len(pixels) < chunkSize:
        pixels += pixels[-1:]
    return pixels


def calculateOutputPixels(state):
    assert isinstance(state, RequestPacket)

    targetScale = ceil(float(state.scale) / RepeatMap.skixelsPerSample)
    if targetScale == 1:
        data = RepeatMap.getBaseRepeatMapData(state)
        pixels = convertRepeatDataToRepeatOverview(state, data, RepeatMap.skixelsPerSample)
    else:
        tempState = copy.deepcopy(state)
        tempState.width = 1
        data = RepeatMap.squishStoredMaps(tempState)
        pixels = convertRepeatDataToRepeatOverview(state, data, state.scale)

    return pixels
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    