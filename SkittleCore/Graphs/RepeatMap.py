'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import NucleotideDisplay
from SkittleGraphTransforms import correlationMap, countDepth, chunkUpList, countNucleotides, normalizeDictionary, countListToColorSpace, pearsonCorrelation
from models import RepeatMapState
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph
import math
from random import choice

registerGraph('m', "Repeat Map", __name__, False)
'''
These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.
'''

def generateRepeatDebugSequence(maxFrequency, bpPerFrequency):
    seq = []
    alphabet = ['A','C','G','T']
    for tandemLength in range(1,maxFrequency+1):
        monomer = ''
        for bp in range(tandemLength):
            monomer += choice(alphabet)
        repeat = ['T']
        while sum(map(lambda x: len(x), repeat)) < bpPerFrequency:
            repeat.append(monomer)
#        print repeat
        seq.append(''.join(repeat))
    
    seq = ''.join(seq)
    return seq

def createScaledColorSequence(state, start, scale):
    chunks = chunkUpList(state.seq[start: start+scale*10], state.scale)
    counts = countNucleotides(chunks)
    counts = normalizeDictionary(counts)
    pixels = countListToColorSpace(counts, state.colorPalette)
    return pixels

def logRepeatMap(state, repeatMapState):
    freq = []
    start = 0
    for h in range(repeatMapState.height(state, state.seq)): # per line
        freq.append( [] )
        scale = 1
        while scale < repeatMapState.F_width:
            #get two scaled sequences
            original = createScaledColorSequence(state, start, scale)
            offsetSequence = createScaledColorSequence(state, start+scale*2, scale) 
            #correlation at offset +1
            
            rgbChannels = zip(*original)
            targetChannels = zip(*offsetSequence)
            resultSum = 0.0
            for index, currentChannel in enumerate(rgbChannels):
                correlation = pearsonCorrelation(currentChannel, targetChannels[index])
                if correlation is not None:
                    resultSum += correlation
            resultSum /= 3
            freq[h].append( .5 * (1.0 + resultSum) )
            scale = int(math.ceil(scale * 1.05))
        start += state.width
    return freq



def countMatches(sequence, beginA, beginB, lineSize):
    matches = 0
    for index in range(lineSize):
        if sequence[beginA + index] == sequence[beginB + index]:
            matches += 1
    return float(matches) / lineSize

def oldRepeatMap(state, repeatMapState):
    freq = []
    lineSize = state.width * state.scale
    for h in range(repeatMapState.height(state, state.seq)):
        freq.append([0.0]*(repeatMapState.F_width+1))
        offset = h * lineSize
        for w in range(1, len(freq[h])):#calculate across widths 1:F_width
            freq[h][w] = countMatches(state.seq, offset, offset + w + repeatMapState.F_start, lineSize)
    return freq


def calculateOutputPixels(state, repeatMapState = RepeatMapState()):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, RequestPacket)
    state.seq = generateRepeatDebugSequence(70, 400)
    
    scores = logRepeatMap(state, repeatMapState)
    return scores
    
    scores = oldRepeatMap(state, repeatMapState)
    return scores
    
    pixels = NucleotideDisplay.calculateOutputPixels(state)
    if countDepth(pixels) > 1:
        singleLine = []
        for x in pixels: #this can't be a list comprehension because we need the += operator instead of .append()
            singleLine += x
    else:
        singleLine = pixels
    scores = correlationMap(state, repeatMapState, singleLine) #2D array
    return scores
    
    
    