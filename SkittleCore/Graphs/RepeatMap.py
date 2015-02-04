'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import math
from random import choice
import copy

from SkittleGraphTransforms import chunkUpList, countNucleotides, normalizeDictionary, countListToColorSpace, pearsonCorrelation, average, composedOfNs
from models import RepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph, handleRequest
from DNAStorage.StorageRequestHandler import GetPngFilePath, GetFastaFilePath
from SkittleCore.png import Reader


registerGraph('m', "Repeat Map", __name__, False, False, 0.4, isGrayScale=True, helpText='''Repeat Map is used for identifying tandem repeats without
 the need for continually adjusting the width in Nucleotide Display.  
 It identifies periodicity of repeated sequences by checking all possible offsets scored by Pearson Correlation displayed in grayscale.  
 The x-axis of the graph represents periodicity, starting at offset 1 on the left and increasing geometrically to offset 6,144 on the right.  
 This growth curve means that Repeat Map can accurately detect 2bp periodicities simultaneously with segmental duplications.  
 Vertical white lines show regions that contain tandem repeats.  Most of the graph will be 25-30% gray from random chance.  
 Black spots are created when two regions with opposite biases are compared as in the case of a CG repeat being compared with an AT repeat region.''')

'''These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.'''
skixelsPerSample = 24


def encodeWidth(nucleotideWidth):
    cumulativeWidth = 0
    megaColumn=0
    subColumn=0

    while cumulativeWidth < (nucleotideWidth-12):
        cumulativeWidth += 2**megaColumn
        subColumn += 1
        if subColumn >= 12:
            subColumn = 0
            megaColumn += 1
    widthPosition = 11 + megaColumn * 12 + subColumn - (cumulativeWidth - nucleotideWidth + 12) / 2.0 **megaColumn

    return int(round(widthPosition))


def decodeWidth(columnIndex):
    """Since RepeatMap using a log scale, this method helps figure out what width (nucleotides per line) a column of the
RepeatMap is point at.  This task is complicated by the duplication of the scale 1 mega column for widths 1-24"""
    cumulativeWidth = 0
    presentColumn = 0
    scalesByMegaColumn = [1,1,2,4,8,16,32,64,128,256,512,1024,2048,4096,]
    for megaColumn in range(len(scalesByMegaColumn)):
        for subColumn in range(12):
            cumulativeWidth += scalesByMegaColumn[megaColumn]
            presentColumn += 1
            if presentColumn > columnIndex:
                return cumulativeWidth
    return 26000  # maximum value


def generateRepeatDebugSequence(maxFrequency, bpPerFrequency, startFrequency=1):
    seq = []
    alphabet = ['A', 'C', 'G', 'T']
    for tandemLength in range(startFrequency, maxFrequency + 1):
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


def sequenceCount(seq, start, scale, end):
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
            print percentCompletion / 10, "% Complete"

        #        skipToNextLine = False
        freq.append([])
        oldScaledSequence = []

        for powerOfX in range(repeatMapState.F_width):
            scale = int(math.ceil(growthPower ** powerOfX))
            if scale * skixelsPerSample >= 64000:#the maximum reach
                break
            end = start + scale * (skixelsPerSample * 2)

            #created scaled sequences
            starterSequence = []
            if scale > 1:
                for step in range(0, len(oldScaledSequence), growthPower):
                    starterSequence.append(
                        reduce(lambda x, y: addDictionaries(x, y), oldScaledSequence[step:step + growthPower], {}))
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
                offsetSequence = scaledSequence[offset: offset + skixelsPerSample]
                validComparison = len(offsetSequence) == len(original) and len(offsetSequence) and len(original)
                if validComparison: #this line is necessary to avoid array index out of bounds or referencing an unsigned variable stretchIsSequenced
                    stretchIsSequenced = not any(map(composedOfNs, offsetSequence))
                    stretchIsSequenced = not any(map(composedOfNs, original)) and stretchIsSequenced
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
                freq[h].append(.66666 * max(0.0, (.5 + resultSum)))

        start += state.nucleotidesPerLine()
    return freq


def checkForCachedMap(state):
    tempState = copy.deepcopy(state)
    tempState.width = skixelsPerSample
    tempState.scale = 1
    tempState.requestedGraph = 'm'

    filepath = GetPngFilePath(tempState)
    return filepath is not None


def conglomeratePNGs(tempState, nChunks):
    fullData = []
    for s in range(nChunks):
        pngPath = GetPngFilePath(tempState)
        if not pngPath:
            if GetFastaFilePath(tempState.specimen, tempState.chromosome, tempState.start) is not None:
                handleRequest(tempState)
                    #disregard the png data returned here since I'd rather read the file consistently
                pngPath = GetPngFilePath(tempState)
            else: #ran out of chunks
                return fullData
        if not pngPath:
            msg = ' '.join(["The request did not create a valid PNG:", tempState.specimen, tempState.chromosome, str(
                tempState.start)])
            open("errors.log", 'a').write(msg)
            raise IOError(msg)
        else:
            decoder = Reader(filename=pngPath)
            fullData += list(decoder.asFloat(1.0)[2])

        tempState.start += chunkSize

    return fullData


def getBaseRepeatMapData(state, repeatMapState=RepeatMapState()):
    #read in the one png at fixed width= skixelsPerSample
    tempState = state.copy() #only preserves specimen and chromosome
    tempState.width = skixelsPerSample
    tempState.scale = 1
    tempState.requestedGraph = 'm'

    return conglomeratePNGs(tempState, state.scale)


def squishStoredMaps(state, repeatMapState=RepeatMapState()):
    fullData = getBaseRepeatMapData(state, repeatMapState)
    #averaging the lines
    newData = []
    nLines = int(math.ceil(state.nucleotidesPerLine() / float(skixelsPerSample)))
    for start in range(0, len(fullData) * skixelsPerSample, state.nucleotidesPerLine()):
        startLine = int(math.floor(start / float(skixelsPerSample)))
        stopLine = startLine + nLines
        sample = zip(*fullData[startLine:stopLine])
        finalLine = [average(x) for x in sample]
        newData.append(finalLine)
    return newData


def calculateOutputPixels(state, repeatMapState=RepeatMapState()):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, RequestPacket)

    if state.nucleotidesPerLine() != skixelsPerSample:
        return squishStoredMaps(state, repeatMapState)

    state.readFastaChunks()#    state.seq = generateRepeatDebugSequence(53, 400, 1)
    scores = logRepeatMap(state, repeatMapState)
    return scores
    
    
    
    