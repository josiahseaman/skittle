'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import math
from random import choice, randint
import copy
from SkittleCore.Graphs.PixelLogic import interpolate

from SkittleGraphTransforms import chunkUpList, countNucleotides, normalizeDictionary, countListToColorSpace, pearsonCorrelation, average, composedOfNs
from models import RepeatMapState, AberrantRepeatMapState
from SkittleCore.models import RequestPacket, chunkSize
from SkittleCore.GraphRequestHandler import registerGraph, handleRequest
from DNAStorage.StorageRequestHandler import GetPngFilePath, GetFastaFilePath
from SkittleCore.png import Reader


registerGraph('x', "48 Repeat Map", __name__, False, False, 0.4, isGrayScale=True, helpText='''Repeat Map is used for identifying tandem repeats without
 the need for continually adjusting the width in Nucleotide Display.  
 It identifies periodicity of repeated sequences by checking all possible offsets scored by Pearson Correlation displayed in grayscale.  
 The x-axis of the graph represents periodicity, starting at offset 1 on the left and increasing geometrically to offset 6,144 on the right.  
 This growth curve means that Repeat Map can accurately detect 2bp periodicities simultaneously with segmental duplications.  
 Vertical white lines show regions that contain tandem repeats.  Most of the graph will be 25-30% gray from random chance.  
 Black spots are created when two regions with opposite biases are compared as in the case of a CG repeat being compared with an AT repeat region.''')

'''These are the functions that are specific to the use of RepeatMap and not generally applicable.  
These functions use RepeatMapState to emulate an object with state.'''


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
                return cumulativeWidth, scalesByMegaColumn[megaColumn]
    return 26000, 4096  # maximum value


def generateRepeatDebugSequence(maxFrequency, bpPerFrequency, startFrequency=1):
    print("generateRepeatDebugSequence", maxFrequency, bpPerFrequency, startFrequency)
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


def validComparison(offsetSequence, originalSeq):
    matchingLength = len(offsetSequence) == len(originalSeq) and len(offsetSequence) and len(originalSeq)
    if matchingLength:  # this line is necessary to avoid array index out of bounds or referencing an unsigned variable regionIsSequenced
        regionIsSequenced = not any(map(composedOfNs, offsetSequence))
        return not any(map(composedOfNs, originalSeq)) and regionIsSequenced
    # this line is necessary to avoid array index out of bounds or referencing an unsigned variable regionIsSequenced
    return matchingLength


def append_mega_column_scores(row, original, scaledSequence, growthPower, repeatMapState, scale):
    rgbChannels = zip(*original)
    # iterate horizontally within a mega-column
    startingOffset = repeatMapState.skixelsPerSample / growthPower
    if scale == 1:
        startingOffset = 1  #TODO: change this to 0 so that there's always 12 pixels per column
    for offset in range(startingOffset, repeatMapState.skixelsPerSample):  # range 12 - 24 but indexing starts at 0
        offsetSequence = scaledSequence[offset: offset + repeatMapState.skixelsPerSample]
        if validComparison(offsetSequence, original):
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
        pixel_value = .66666 * max(0.0, (.5 + resultSum))
        row.append(pixel_value)


def logRepeatRow(repeatMapState, start, state, prevRows):
    growthPower = 2
    end = repeatMapState.height(state, state.seq) * state.nucleotidesPerLine()
    lastLine = start + state.nucleotidesPerLine() >= end
    row = []
    oldScaledSequence = []
    for megaColumn in range(repeatMapState.F_width):  # mega columns
        scale = int(math.ceil(growthPower ** megaColumn))
        if scale * repeatMapState.skixelsPerSample > 64000:  # the maximum reach should be less than 1 chunk
            break
        if megaColumn > 2 and len(prevRows) % (scale/4) != 0 and not lastLine:  # don't pad the last line (it makes the boundary obvious)
            # column_start = len(row)
            row += [None] * (repeatMapState.skixelsPerSample / growthPower )
            # row += prevRows[-1][column_start: column_start + repeatMapState.skixelsPerSample / growthPower ]  # append slice from row above
        else:
            end = start + scale * repeatMapState.skixelsPerSample * 2
    
            # created scaled sequences
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
    
            original = scaledSequence[0:repeatMapState.skixelsPerSample]
            append_mega_column_scores(row, original, scaledSequence, growthPower, repeatMapState, scale)
    return row


def find_value(freq, x, y, stepUp=True):
    if stepUp:
        for row in range(y, -1, -1):  #includes 0 index, but not -1
            if freq[row][x] is not None:
                return row, freq[row][x]
    else:
        for row in range(y, len(freq), 1):
            if freq[row][x] is not None:
                return row, freq[row][x]
    return len(freq) * stepUp, 0


def interpolate_the_gaps(freq, repeatMapState):
    for y, row in enumerate(freq):
        for x, pixel in enumerate(row):
            if pixel is None:
                above_index, above_value = find_value(freq, x, y, stepUp=True)
                below_index, below_value = find_value(freq, x, y, stepUp=False)
                freq[y][x] = interpolate(above_value, below_value, above_index, below_index, y)
    return freq


def logRepeatMap(state, repeatMapState):
    freq = []
    state.readAndAppendNextChunk()
    print "Done reading additional chunk.  Computing..."
    end = repeatMapState.height(state, state.seq) * state.nucleotidesPerLine()
    for start in range(0, end, state.nucleotidesPerLine()): # per line
        percentCompletion = int(float(start) / end * 100)
        if randint(0,50) == 0:
            print percentCompletion, "% Complete"

        row = logRepeatRow(repeatMapState, start, state, freq)
        freq.append(row)
        print len(freq), " rows Complete"
    freq = interpolate_the_gaps(freq, repeatMapState)
    return freq


def checkForCachedMap(state, repeatMapState):  # TODO: Possibly dead code
    tempState = copy.deepcopy(state)
    tempState.width = repeatMapState.skixelsPerSample
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
    tempState.width = repeatMapState.skixelsPerSample
    tempState.scale = 1
    tempState.requestedGraph = 'x'

    return conglomeratePNGs(tempState, state.scale)


def squishStoredMaps(state, repeatMapState=RepeatMapState()):
    fullData = getBaseRepeatMapData(state, repeatMapState)
    #averaging the lines
    newData = []
    nLines = int(math.ceil(state.nucleotidesPerLine() / float(repeatMapState.skixelsPerSample)))
    for start in range(0, len(fullData) * repeatMapState.skixelsPerSample, state.nucleotidesPerLine()):
        startLine = int(math.floor(start / float(repeatMapState.skixelsPerSample)))
        stopLine = startLine + nLines
        sample = zip(*fullData[startLine:stopLine])
        finalLine = [average(x) for x in sample]
        newData.append(finalLine)
    return newData


def calculateOutputPixels(state, repeatMapState=AberrantRepeatMapState()):
    assert isinstance(repeatMapState, AberrantRepeatMapState)
    assert isinstance(state, RequestPacket)

    if state.nucleotidesPerLine() != repeatMapState.skixelsPerSample:
        return squishStoredMaps(state, repeatMapState)

    # state.seq = generateRepeatDebugSequence(25, 1000, 1)
    state.readFastaChunks()#    state.seq = generateRepeatDebugSequence(53, 400, 1)
    scores = logRepeatMap(state, repeatMapState)
    return scores
    
    
    
    