from __future__ import print_function
from collections import namedtuple
import math
from SkittleCore.GraphRequestHandler import registerGraph
from SkittleCore.Graphs.RepeatMap import skixelsPerSample, decodeWidth, encodeWidth, getBaseRepeatMapData
from SkittleCore.Graphs.SkittleGraphTransforms import sequenceToColors
from SkittleCore.models import RequestPacket, chunkSize

__author__ = 'Josiah'

registerGraph('p', "Photo Gallery", __name__, False, True, stretchy=False, helpText='''A graph to show sequence snippets that match
a detected tandem repeat.''')


class Snippet():
    def __init__(self, start, stop, width):
        self.start, self.stop, self.width = start, stop, width
        while self.width < 25:
            if self.width == 0:
                self.width = 13
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
        assert len(line) == maxWidth
        pixels.append(line)
    return pixels


def deleteBlankLine(pixels):
    startSize = len(pixels)
    for lineNumber in range(len(pixels) - 1, -1, -1):  # reverse iteration without rearranging the list
        if not any(pixels[lineNumber]):
            pixels.pop(lineNumber)
            return
    raise StopIteration("There are no more blank lines to delete")
    return


def padForHorizontalLineSynchronization(referenceWidth, lineWidth, pixels, start):
    while len(pixels) > math.ceil(float(start) / referenceWidth):
        try:
            deleteBlankLine(pixels)
        except StopIteration:
            break

    while len(pixels) < math.ceil(float(start) / referenceWidth):
        assert not pixels or len(pixels[-1]) == lineWidth
        pixels.append([None] * lineWidth)
    return pixels


def findBestFrequency(repeatMapLine):
    highestScore = max(repeatMapLine)
    frequency = decodeWidth(repeatMapLine.index(highestScore))
    for index, score in enumerate(repeatMapLine):
        if score > (highestScore * 0.7):
            frequency = decodeWidth(index)
            break
    return frequency


def createSnippetEntries(repeatMap, state):
    creatingNewSnippet = False
    snippets = []
    startSnippetThreshold = 0.8
    maintainThreshold = 0.5
    previousSnippetEnd = -1
    repeatMap = map(lambda line: line[:len(line) / 2], repeatMap) #only use the first half
    for lineIndex, line in enumerate(repeatMap):
        if not creatingNewSnippet and max(line) >= startSnippetThreshold:
            start = lineIndex * skixelsPerSample
            if state.seq[start] == 'N':
                continue
            creatingNewSnippet = True
            width = findBestFrequency(line)
        if creatingNewSnippet and max(line) < maintainThreshold:
            creatingNewSnippet = False
            snippets.append(Snippet(start, (lineIndex + 1) * skixelsPerSample, width))
    return snippets


def snippetEntriesToPixels(maxWidth, snippets, state):
    pixels = [] if snippets else [None]  # defaults to single blank pixel
    for snippet in snippets:
        padForHorizontalLineSynchronization(state.nucleotidesPerLine(), maxWidth, pixels, snippet.start)
        pixels += arrangePixels(state, snippet, maxWidth)
    padForHorizontalLineSynchronization(state.nucleotidesPerLine(), maxWidth, pixels, chunkSize)
    return pixels


def calculateOutputPixels(state):
    state.readFastaChunks()
    assert isinstance(state, RequestPacket)

    repeatMap = getBaseRepeatMapData(state)
    snippets = createSnippetEntries(repeatMap, state)

    widths = map(lambda x: x.width, snippets)
    maxWidth = 1 if len(widths) == 0 else max(widths)
    print("Max width is", maxWidth, "over", len(snippets), "snippets")

    pixels = snippetEntriesToPixels(maxWidth, snippets, state)

    return pixels


