'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from Utilities.debug import startDebug
import NucleotideDisplay
from SkittleGraphTransforms import correlationMap, countDepth, chunkUpList, countNucleotides, normalizeDictionary, countListToColorSpace, pearsonCorrelation, average
from models import RepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph
import math
from random import choice
from DNAStorage.StorageRequestHandler import GetPngFilePath
from SkittleCore.png import Reader
from SkittleCore.PngConversionHelper import convertToPng

registerGraph('m', "Repeat Map", __name__, False, False, 0.4)
skixelsPerSample = 24
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

def sequenceCount(seq, start, scale, end ):
    if len(seq[start: end]) == 0:
        return []
    chunks = chunkUpList(seq[start: end], scale)
    counts = countNucleotides(chunks)
    return counts

def colorizeSequence(counts):
    counts = normalizeDictionary(counts)
    pixels = countListToColorSpace(counts, 'Classic')
    return pixels

def addDictionaries(jim, larry):
    assert isinstance(jim, dict) and isinstance(larry, dict)
    newDict = jim #so that keys that are in jim but not larry are still copied
    for key in larry.keys():
        newDict[key] = jim.get(key, 0) + larry[key]
    return newDict

def logRepeatMap(state, repeatMapState):
    freq = []
    start = 0
    growthPower = 2
    height = repeatMapState.height(state, state.seq)
    state.readAndAppendNextChunk()
    print "Done reading additional chunk.  Computing..."
    for h in range(height): # per line
        percentCompletion = int(float(h) / height * 1000)
        if percentCompletion % 100 == 0:
            print percentCompletion/10, "% Complete"
            
#        skipToNextLine = False
        freq.append( [] )
        oldScaledSequence = []
        
        for powerOfX in range(repeatMapState.F_width):
            scale = int(math.ceil(growthPower ** powerOfX))
            if scale * skixelsPerSample >= 64000 :#the maximum reach
                break
            end = start + scale*(skixelsPerSample*2)
            
            
            #check if we have changed start enough to update the score 
            #equation = h % ceil( tolerance / (increment / (skixelsPerSample*scale)))
            ''' 
            needComputation = h  % int(max(1.0, math.ceil( scale * 0.05))) == 0 #precomputed value 0.2 based on 10% of 24 skixels 
            if not needComputation: #otherwise, use previous score
                subColumnsPerMegaColumn = skixelsPerSample - skixelsPerSample / growthPower
                previousLine = freq[ len(freq)-2 ][powerOfX*subColumnsPerMegaColumn: ] #reference previous display line
                freq[h] += previousLine
#                for i in range(powerOfX * subColumnsPerMegaColumn, (powerOfX+1)* subColumnsPerMegaColumn):
#                    freq[h].append(previousLine[i])
                skipToNextLine = True
                break
            else:
                if skipToNextLine:
                    print "Improper Skip: Start: ", start, "  Scale: ", scale
            if skipToNextLine:
                skipToNextLine = False
                break                
           ''' 
             
            #created scaled sequences
            starterSequence = []
            if scale > 1:
                for step in range(0, len(oldScaledSequence), growthPower):
                    starterSequence.append(reduce(lambda x,y: addDictionaries(x,y), oldScaledSequence[step:step+growthPower], {}))
            necessaryStart = start + len(starterSequence) * scale
            try:
                scaledSequence = starterSequence + sequenceCount(state.seq, necessaryStart, scale, end)
            except:
                scaledSequence = starterSequence + [sequenceCount(state.seq, necessaryStart, scale, end)]
            oldScaledSequence = scaledSequence
            scaledSequence = colorizeSequence(scaledSequence)

#            scaledSequence = colorizeSequence(sequenceCount(state.seq, start, scale, end))

            original = scaledSequence[0:skixelsPerSample]
            rgbChannels = zip(*original)
            
            #iterate horizontally within a mega-column
            for offset in range(skixelsPerSample/growthPower, skixelsPerSample): #range 5 - 12 but indexing starts at 0
                offsetSequence = scaledSequence[offset : offset + skixelsPerSample]
                if len(offsetSequence) == len(original):
                    targetChannels = zip(*offsetSequence)
                    resultSum = 0.0
                    for index, currentChannel in enumerate(rgbChannels):
                        correlation = pearsonCorrelation(currentChannel, targetChannels[index])
                        if correlation is not None:
                            resultSum += correlation
                    resultSum /= 3
#                    if resultSum > 0.9:
#                        print (scale, scale * offset)
                else:
                    resultSum = -1.0
                freq[h].append( .66666 * max(0.0, (.5 + resultSum)) )

        start += state.nucleotidesPerLine()
    return freq



def countMatches(sequence, beginA, beginB, lineSize):
    matches = 0
    for index in range(lineSize):
        if sequence[beginA + index] == sequence[beginB + index]:
            matches += 1
    return float(matches) / lineSize

def oldRepeatMap(state, repeatMapState):
    freq = []
    lineSize = state.nucleotidesPerLine()
    for h in range(repeatMapState.height(state, state.seq)):
        freq.append([0.0]*(repeatMapState.F_width+1))
        offset = h * lineSize
        for w in range(1, len(freq[h])):#calculate across widths 1:F_width
            freq[h][w] = countMatches(state.seq, offset, offset + w + repeatMapState.F_start, lineSize)
    return freq

def squishStoredMaps(state, repeatMapState):
    #read in the one png at fixed width= skixelsPerSample
    oldWidth = state.width * state.scale
    state.width = skixelsPerSample
    state.scale = 1
    filepath = GetPngFilePath(state)
    data = []
    if filepath:
        decoder = Reader(filename=filepath)
        data = list(decoder.asFloat(1.0)[2])
    else:
        data = calculateOutputPixels(state, repeatMapState)
        convertToPng(state, data )#store the newly created data to file
    state.width = oldWidth
        
    print "REPEAT MAP DATA!!"
#    print data
    
    #averaging the lines
    newData = []
    nLines = int(math.ceil(oldWidth / float(skixelsPerSample)))
    for start in range(0, chunkSize, oldWidth):
        startLine = int(math.floor( start / float(skixelsPerSample)))
        stopLine = startLine + nLines
        sample = zip(*data[startLine:stopLine])
        finalLine = [average(x) for x in sample]
        newData.append(finalLine)
    return newData

def calculateOutputPixels(state, repeatMapState = RepeatMapState()):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, RequestPacket)

    if state.nucleotidesPerLine() != skixelsPerSample:
        return squishStoredMaps(state, repeatMapState)


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
    
    
    