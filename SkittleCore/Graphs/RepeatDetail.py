'''
Created on Feb, 2015
@author: Josiah
'''
from SkittleGraphTransforms import chunkUpList, normalizeDictionary, countListToColorSpace, sequenceToColors, countNucleotides
from SkittleCore.models import RequestPacket
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('d', "RepeatMap Detail", __name__, False, True, helpText='''The exact sequence pair comparison that led to
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


def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)
    #    chunks = chunkUpList(state.seq, state.nucleotidesPerLine() )
    
    # determine the offset in nucleotides
    start_seq = state.seq[state.start : state.start + state.nucleotidesPerLine()]
    second = state.start + state.width
    second_seq = state.seq[second : second + state.nucleotidesPerLine()]


    # append two slices of the sequence
    state.seq = [start_seq, second_seq]
    # colorize

    if False: #state.scale > 1:
        chunks = chunkUpList(state.seq, state.scale)
        counts = countNucleotides(chunks)
        counts = normalizeDictionary(counts)
        pixels = countListToColorSpace(counts, state.colorPalette, state.scale)
    else:
        pixels = sequenceToColors(state.seq, state.colorPalette)
    return pixels
    

    
    
    