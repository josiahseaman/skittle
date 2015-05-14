'''
Created on Dec 5, 2012
@author: Josiah
'''
import collections
from random import randint
import SkittleCore.models as SkittleRequestPackets
import math
from SkittleCore.models import chunkSize


blankColor = None

colorPalettes = {
    "COLORBLINDSAFE":
        {'A': (255, 102, 0),
         'C': (153, 0, 0),
         'G': (51, 255, 51),
         'T': (0, 153, 204),
         'N': ( 200, 200, 200)}, #not sequenced

    "BETTERCBSAFE":
        {'A': (0, 204, 204),
         'C': (153, 255, 0),
         'G': (204, 0, 102),
         'T': (255, 102, 0),
         'N': ( 200, 200, 200)}, #not sequenced

    "DARK":
        {'A': (152, 147, 173),
         'C': (84, 40, 59),
         'G': (13, 94, 58),
         'T': (40, 75, 163),
         'N': ( 200, 200, 200)}, #not sequenced

    "DRUMS":
        {'A': (80, 80, 255),
         'C': (224, 0, 0),
         'G': (0, 192, 0),
         'T': (230, 230, 0),
         'N': ( 200, 200, 200)}, #not sequenced

    "BLUES":
        {'A': (141, 0, 74),
         'C': (82, 0, 124),
         'G': (17, 69, 134),
         'T': (14, 112, 118),
         'N': ( 200, 200, 200)}, #not sequenced
    "REDS":
        {'A': (141, 0, 74),
         'C': (159, 0, 0),
         'G': (196, 90, 6),
         'T': (218, 186, 8),
         'N': ( 200, 200, 200)}, #not sequenced

    "Classic":
        {'A': (0, 0, 0),
         'C': (255, 0, 0),
         'G': (0, 255, 0),
         'T': (0, 0, 255),
         'N': ( 200, 200, 200), #not sequenced
         '_': (140, 140, 140),
         'I': (255, 0, 255),
         'D': (0, 200, 200)}
}
'''Safe accessor for nucleotide colors'''


def getColor(state, character):
    assert isinstance(state, SkittleRequestPackets.RequestPacket)
    defaultColor = (0, 0, 0)
    a = colorPalettes.get(state.colorPalette, None)
    if a is None:
        print "Palette not found"
        return defaultColor
    b = a.get(character, defaultColor)
    return b


'''Returns a color tuple with one rgb channel set to a random value'''


def randomColor():#only changes one channel right now
    colorT = [0, 0, 0]
    randomChannel = randint(0, 2)
    colorT[randomChannel] = randint(0, 255)
    return tuple(colorT)


def blackSquare():
    return [(0, 0, 0)] * chunkSize


def blackSquareGrayScale():
    return [0.0] * chunkSize


def drawBar(size, filler_size, barColor, rightJustified, drawBackgroundGray=False):
    if drawBackgroundGray:
        filler = [(51, 51, 51)] * max(0, filler_size)
    else:
        filler = [None] * max(0, filler_size)
    bar = [barColor] * size
    line = []
    if rightJustified:
        line += filler
        line += bar
    else:
        line += bar
        line += filler
    return line


def drawJustifiedBar(barSizes, colorSeries, max_bar_width, drawBackgroundGray=False):
    assert len(barSizes) == len(colorSeries)
    line = []
    for position in range(len(barSizes)):
        size = barSizes[position]
        modulo = position % 2
        filler_size = 0
        rightJustified = (modulo == 0)

        #        switch(position)//this is the order in which the nucleotides are displayed
        #        {//the two pointing outwards need twice the margin of the inner two because
        #        //the inner two can overlap each other.
        if modulo == 0:
            if position == 0:
                filler_size = max_bar_width - size
        elif modulo == 1:
            filler_size = max_bar_width - size
            if position + 1 < len(barSizes):
                filler_size -= barSizes[position + 1] #overflow from the next letter

        barColor = colorSeries[position]
        line += drawBar(size, filler_size, barColor, rightJustified, drawBackgroundGray)
    assert len(line) == max_bar_width * math.ceil(len(barSizes) / 2.0), "Result %i shoulld be %i." % (
    len(line), max_bar_width * math.ceil(len(barSizes) / 2.0))
    return line


def hasDepth(listLike):
    try:
        return len(listLike) > 0 and not isinstance(listLike, (str, dict, tuple, type(u"unicode string"))) and hasattr(
            listLike[0], "__getitem__")
    except:
        return False


def interpolate(A, B, start, end, position):
    if start == end:
        return A
    progress = (position - start) / float(end - start)#progress goes from 0.0 p1  to 1.0 p2
    inverse = 1.0 - progress;
    if isinstance(A, collections.Iterable):
        x2 = A[0] * inverse + B[0] * progress;
        y2 = A[1] * inverse + B[1] * progress;
        z2 = A[2] * inverse + B[2] * progress;
        return (x2, y2, z2)
    else:
        return A * inverse + B * progress


def __colorByCustomSpectrum(spectrumDict, position):
    assert isinstance(spectrumDict, dict)
    below = min(spectrumDict.keys())
    above = max(spectrumDict.keys())
    try:
        below = max(filter(lambda point: point <= position, spectrumDict.keys()))
    except:
        pass
    try:
        above = min(filter(lambda point: point >= position, spectrumDict.keys()))
    except:
        pass

    return interpolate(spectrumDict[below], spectrumDict[above], below, above, position)


def spectrum(floatingPoint):
    spectrumPoints = {0.0: (0, 0, 255), 0.25: (255, 0, 0), 0.5: (255, 255, 0), 0.75: (0, 255, 0), 1.0: (0, 255, 255)}
    return __colorByCustomSpectrum(spectrumPoints, floatingPoint)


def twoSidedSpectrumColoring(floatList, midpoint=0.0):
    if hasDepth(floatList):
        return map(lambda x: twoSidedSpectrumColoring(x, midpoint), floatList)
    pixels = []
    purple, blue, black, red, white = (125, 0, 255), (0, 0, 255), (0, 0, 0), (255, 0, 0), (255, 255, 255)
    maxVal = 1.0
    highMid = 0.965 #(5*maxVal + midpoint) / 6
    minVal = -1.0
    lowMid = (minVal + midpoint) / 2
    for score in floatList:
        if score is None:
            pixels.append((0, 0, 0))
        elif score > highMid:
            pixels.append(interpolate(red, white, highMid, maxVal, score))
        elif score <= highMid and score > midpoint:
            pixels.append(interpolate(black, red, midpoint, highMid, score))
        elif score <= midpoint and score > lowMid:
            pixels.append(interpolate(blue, black, lowMid, midpoint, score))
        elif score <= lowMid:
            pixels.append(interpolate(purple, blue, minVal, lowMid, score))

    return pixels