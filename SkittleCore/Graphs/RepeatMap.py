'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
from Utilities.debug import startDebug
from SkittleGraphTransforms import  chunkUpList, countNucleotides, normalizeDictionary, countListToColorSpace, pearsonCorrelation, average, composedOfNs
from models import RepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph, handleRequest
import math
from random import choice
from DNAStorage.StorageRequestHandler import GetPngFilePath, GetFastaFilePath
from SkittleCore.png import Reader
from SkittleCore.PngConversionHelper import convertToPng
import copy

registerGraph('m', "Repeat Map", __name__, False, False, 0.4, isGrayScale=True, helpText='Repeat Map is used for identifying tandem repeats without the need for continually adjusting the width in Nucleotide Display.  It identifies periodicity of repeated sequences by checking all possible offsets and scoring 0-100% similarity displayed in grayscale.  The x-axis of the graph represents periodicity, starting at offset 1 on the left and increasing geometrically to offset 6,144 on the right.  This growth curve means that Repeat Map can accurately detect 2bp periodicities simultaneously with segmental duplications.  Vertical white lines show regions that contain tandem repeats.  Most of the graph will be 25-30% gray from random chance.  Black spots are created when two regions with opposite biases are compared as in the case of a CG repeat being compared with an AT repeat region.')
'''
These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.
'''
skixelsPerSample = 24

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

def colorizeSequence(counts, scale):
    counts = normalizeDictionary(counts)
    pixels = countListToColorSpace(counts, 'Classic', scale)
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
            scaledSequence = colorizeSequence(scaledSequence, scale)

#            scaledSequence = colorizeSequence(sequenceCount(state.seq, start, scale, end))

            original = scaledSequence[0:skixelsPerSample]
            rgbChannels = zip(*original)
            
            #iterate horizontally within a mega-column
            startingOffset = skixelsPerSample / growthPower
            if scale == 1:
                startingOffset = 1
            for offset in range(startingOffset, skixelsPerSample): #range 12 - 24 but indexing starts at 0
                offsetSequence = scaledSequence[offset : offset + skixelsPerSample]
                validComparison = len(offsetSequence) == len(original) and len(offsetSequence) and len(original)
                if validComparison: #this line is necessary to avoid array index out of bounds or referencing an unsigned variable stretchIsSequenced
                    stretchIsSequenced = not any( map(composedOfNs, [offsetSequence[0], offsetSequence[-1:][0], original[0], original[-1:][0] ]))  
                if validComparison and stretchIsSequenced: #this line is necessary to avoid array index out of bounds or referencing an unsigned variable stretchIsSequenced
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

def checkForCachedMap(state):
    tempState = copy.deepcopy(state)
    tempState.width = skixelsPerSample
    tempState.scale = 1
    tempState.requestedGraph = 'm'

    filepath = GetPngFilePath(tempState)
    return filepath is not None

def getBaseRepeatMapData(state, repeatMapState = RepeatMapState()):
    #read in the one png at fixed width= skixelsPerSample
    tempState = copy.deepcopy(state)
    tempState.width = skixelsPerSample
    tempState.scale = 1
    tempState.requestedGraph = 'm'
    
    fullData = []
    for s in range(state.scale):
        filepath = GetPngFilePath(tempState)
        if not filepath:
            if GetFastaFilePath(tempState.specimen, tempState.chromosome, tempState.start) is not None:
                handleRequest(tempState) #disregard the png data returned here since I'd rather read the file consistently
                filepath = GetPngFilePath(tempState)
        if not filepath:
            msg = "The request was for an invalid fasta file:" + tempState.specimen+ tempState.chromosome+ str(tempState.start)
            open("errors.log",'a').write(msg)
            raise IOError(msg)
        decoder = Reader(filename=filepath)
        fullData += list(decoder.asFloat(1.0)[2])
        
        tempState.start += chunkSize
    return fullData 
    

def squishStoredMaps(state, repeatMapState = RepeatMapState()):
    fullData = getBaseRepeatMapData(state, repeatMapState)
    #averaging the lines
    newData = []
    nLines = int(math.ceil(state.nucleotidesPerLine() / float(skixelsPerSample)))
    for start in range(0, len(fullData) * skixelsPerSample, state.nucleotidesPerLine()):
        startLine = int(math.floor( start / float(skixelsPerSample)))
        stopLine = startLine + nLines
        sample = zip(*fullData[startLine:stopLine])
        finalLine = [average(x) for x in sample]
        newData.append(finalLine)
    return newData

def calculateOutputPixels(state, repeatMapState = RepeatMapState()):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, RequestPacket)

    if state.nucleotidesPerLine() != skixelsPerSample:
        return squishStoredMaps(state, repeatMapState)

    state.readFastaChunks()#    state.seq = generateRepeatDebugSequence(53, 400, 1)
    scores = logRepeatMap(state, repeatMapState)
    return scores
    
    
    
    