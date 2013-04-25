from collections import namedtuple
import math
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.Graphs.RepeatMap import skixelsPerSample, decodeWidth, getBaseRepeatMapData
from SkittleCore.Graphs.SkittleGraphTransforms import sequenceToColors
from SkittleCore.models import RequestPacket, chunkSize

__author__ = 'Josiah'

registerGraph('p', "Photo Gallery", __name__, False, True, helpText='''A graph to show sequence snippets that match
a detected tandem repeat.''')

class Snippet():
    def __init__(self, start, stop, width):
        self.start, self.stop, self.width = start, stop, width
        while self.width < 15:
            self.width *= 2
    def __repr__(self):
        return str((self.start, self.stop, self.width))


def arrangePixels(state, snippet, maxWidth):
    assert isinstance(snippet, Snippet)
    pixels = []
    seqStart = snippet.start
    for lineStartIndex in range(seqStart, snippet.stop, snippet.width):
        line = []
        line += sequenceToColors(state.seq[lineStartIndex:lineStartIndex + snippet.width], state.colorPalette)
        line += [None] * (maxWidth - snippet.width)  # pad line
        pixels.append(line)
    return pixels


def padLinesTilStartPoint(state, pixels, start):
    while len(pixels) < math.ceil(start / state.nucleotidesPerLine()):
        pixels.append([None] * state.nucleotidesPerLine())
    return pixels

def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)

    repeatMap = getBaseRepeatMapData(state)
    littleRM = repeatMap[:20]
    littleRM = map(lambda line: line[:len(line)/2], littleRM)
    creatingNewSnippet = False
    snippets = []
    startSnippetThreshold = 0.8
    maintainThreshold = 0.5
    for lineIndex, line in enumerate(repeatMap):
        line = line[:len(line)/2]
        if not creatingNewSnippet and max(line) >= startSnippetThreshold:
            creatingNewSnippet = True
            start = lineIndex * skixelsPerSample
            width = decodeWidth(line.index(max(line)))
        if creatingNewSnippet and max(line) < maintainThreshold:
            creatingNewSnippet = False
            snippets.append(Snippet(start, (lineIndex + 1) * skixelsPerSample, width))

    widths = map(lambda x: x.width, snippets)
    maxWidth = max(widths)
    print "Max width is", maxWidth

    pixels = []
    for snippet in snippets:
        padLinesTilStartPoint(state, pixels, snippet.start)
        pixels += arrangePixels(state, snippet, maxWidth)
    padLinesTilStartPoint(state, pixels, chunkSize)
    print "N pixels: ", len(pixels)

    return pixels


