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
from random import choice, randrange

registerGraph('m', "Repeat Map", __name__, False)
'''
These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.
'''

def generateRepeatDebugSequence(maxFrequency, bpPerFrequency, startFrequency = 1):
    seq = []
    alphabet = ['A','C','G','T']
    for tandemLength in range(startFrequency, maxFrequency+1):
        tandemLength = int(tandemLength ** 1.2) 
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

def createScaledColorSequence(seq, start, scale, end = -1):
    if end == -1: end = start+scale*10
    chunks = chunkUpList(seq[start: end], scale)
    counts = countNucleotides(chunks)
    counts = normalizeDictionary(counts)
    pixels = countListToColorSpace(counts, 'Classic')
    return pixels

def logRepeatMap(state, repeatMapState):
    freq = []
    start = 0
    skixelOffset = 5
    for h in range(repeatMapState.height(state, state.seq)): # per line
        freq.append( [] )
        scale = 1
        while scale < repeatMapState.F_width:
            end = start + scale*15
            scaledSequence = createScaledColorSequence(state.seq, start, scale, end)
            assert len(scaledSequence) == 16, scaledSequence
            #get two scaled sequences
            original = scaledSequence[0:10]
            rgbChannels = zip(*original)
            
            offsetSequence = scaledSequence[4:14]
            targetChannels = zip(*offsetSequence)
            resultSum = 0.0
            for index, currentChannel in enumerate(rgbChannels):
                correlation = pearsonCorrelation(currentChannel, targetChannels[index])
                if correlation is not None:
                    resultSum += correlation
            resultSum /= 3
            if resultSum > 0.9:
                print (scale, scale * skixelOffset)
            freq[h].append( .5 * (1.0 + resultSum) )
            scale = scale+1  #int(math.ceil(scale * 1.05))
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
#    state.seq = generateRepeatDebugSequence(53, 400, 1)
    
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
    
    
    