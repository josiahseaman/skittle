'''
Created on Feb, 2015
@author: Josiah
'''
import math
from SkittleCore.Graphs.RepeatMap import decodeWidth
from SkittleCore.Graphs.models import RepeatMapState
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('d', "RepeatMap Detail", __name__, False, True, widthTolerance=0, helpText='''The exact sequence pair comparison that led to
each pixel of the RepeatMap is shown.''')
# 
# def countMatches(sequence, beginA, beginB, lineSize):
#     matches = 0
#     for index in range(lineSize):
#         if sequence[beginA + index] == sequence[beginB + index]:
#             matches += 1
#     return float(matches) / lineSize
# 
# 
# def oldRepeatMap(state, threeMerState):
#     assert isinstance(threeMerState, ThreeMerDetectorState)
#     freq = []
#     lineSize = state.nucleotidesPerLine()
#     for h in range(threeMerState.height(state, state.seq)):
#         freq.append([0.0] * (threeMerState.samples * 3 + 1))
#         offset = h * lineSize
#         for w in range(1, len(freq[h])):
#             freq[h][w] = countMatches(state.seq, offset, offset + w, lineSize)
#     return freq


def determineOffset(repeatMapState, state):
    # determine the offset in nucleotides.  This is the inverse function of draw_canvas.js:graphStatus['m'].drawPixelPost = function() {}
    megaColumn = max(0, int((repeatMapState.offsetColumn-11) / 12))  # can't use state.width because it gets max(12, width)
    subcolumn = repeatMapState.offsetColumn - megaColumn * 12
    state.scale = 2 ** megaColumn
    offset = (12 + subcolumn) * state.scale  # ranges from 12 to 24
    if megaColumn == 0:
        offset = repeatMapState.offsetColumn + 1
    end = state.relativeStart + state.scale * (repeatMapState.skixelsPerSample * 2)
    return offset


def calculateOutputPixels(state, repeatMapState=RepeatMapState()):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)
    #    chunks = chunkUpList(state.seq, state.nucleotidesPerLine() )

    if repeatMapState.duplicateFirstColumn:
        offset, state.scale = decodeWidth(repeatMapState.offsetColumn)
    else:
        offset = determineOffset(repeatMapState, state)
    
    # state.relativeStart made by substracting start of chunk position
    samples = repeatMapState.skixelsPerSample * state.scale
    start_seq = state.seq[state.relativeStart : state.relativeStart + samples]
    second = state.relativeStart + offset
    second_seq = state.seq[second : second + samples]

    state.seq = [start_seq, second_seq]  # append two slices of the sequence

    if state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
        counts = countNucleotides(chunks)
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette, state.scale)
        #TODO: colors could be modified to highlight matches.  Any pixel that is not a match, average with 50% gray, that will make the matches stand out
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
        #TODO: zoomed out sequence could be followed by scale 1 colored sequence.  You'll need to manually wrap and interlace lines to get them on top of
        #each other.  This wouldln't be literally contiguous sequence but it would communicate correctly.  Use alpha=1 pixel line to create separators. 
    return pixels
    

    
    
    