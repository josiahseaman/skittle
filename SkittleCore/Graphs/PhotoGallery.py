from collections import namedtuple
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.Graphs.RepeatMap import skixelsPerSample, decodeWidth, getBaseRepeatMapData
from SkittleCore.Graphs.SkittleGraphTransforms import sequenceToColors
from SkittleCore.models import RequestPacket

__author__ = 'Josiah'

registerGraph('p', "Photo Gallery", __name__, False, True, helpText='''A graph to show sequence snippets that match
a detected tandem repeat.''')

class Snippet():
    def __init__(self, start, stop, width):
        self.start, self.stop, self.width = start, stop, width
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


def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)

    repeatMap = getBaseRepeatMapData(state)
    creatingNewSnippet = False
    snippets = []
    threshold = 0.9
    for lineIndex, line in enumerate(repeatMap):
        if max(line) >= threshold:
            creatingNewSnippet = True
            start = lineIndex * skixelsPerSample
        if creatingNewSnippet and max(line) < threshold:
            width = decodeWidth(line.index(max(line)))
            snippets.append(Snippet(start, lineIndex * skixelsPerSample, width))
            creatingNewSnippet = False
            print "Finished a snippet"

    widths = map(lambda x: x.width, snippets)
    maxWidth = max(widths)
    print "Max width is", maxWidth

    pixels = []
    for snippet in snippets:
        pixels += arrangePixels(state, snippet, maxWidth)
    print "N pixels: ", len(pixels)
    for line in pixels:
        print line
    return pixels


