'''
Created on Nov 29, 2012
@author: Josiah Seaman 
'''
#import numpy
import math
from math import sqrt
from numbers import Number
import ctypes

from models import ThreeMerDetectorState
from PixelLogic import colorPalettes


try:
    skittleUtils = ctypes.CDLL(
        'D:/bryan/Documents/Projects/SkittleTree/SkittleCore/Graphs/libSkittleGraphUtils.so.1.0.0')
    usingCcode = True
except:
    usingCcode = False


def countDepth(listLike):
    count = 0
    element = listLike
    while hasDepth(element):
        count += 1
        element = listLike[0]
    return count


def hasDepth(listLike):
    try:
        if len(listLike) > 0 and not isinstance(listLike, (str, dict, tuple, type(u"unicode string"))) and hasattr(
                listLike[0], "__getitem__"):
        #            print 'recursing length: ', len(listLike)
            return True
        else:
            return False
    except:
        return False


def getChunkStart(arbitraryStart):
    if hasDepth(arbitraryStart):
        return map(lambda x: getChunkStart(x), arbitraryStart)
    arbitraryStart = int(arbitraryStart)
    chunkNumber = int(arbitraryStart / 2 ** 16)
    return chunkNumber * (2 ** 16) + 1


def mirrorDiagonalMatrix(heatMap):
    for y in range(len(heatMap)):
        for x in range(y + 1, len(heatMap[y])):#only iterates on the upper right half
            heatMap[x][y] = heatMap[y][x]


def rowColumnCorrelation(heatMap):
    neighborCorrelation = [[None for x in range(len(heatMap[y]))] for y in range(len(heatMap))]
    columnsList = zip(*heatMap)
    for y, rowY in enumerate(heatMap):
        for x in range(y, len(columnsList[y])):
            columnX = columnsList[x]
            neighborCorrelation[y][x] = pearsonCorrelation(columnX, rowY)
    mirrorDiagonalMatrix(neighborCorrelation)
    return neighborCorrelation


def generateExhaustiveOligomerList(oligomerSize, startingSet=[]):
    letters = ['A', 'C', 'G', 'T']
    if not startingSet:
        startingSet = letters
    if len(startingSet[0]) < oligomerSize:
        newSet = []
        for olig in startingSet:
            for n in letters: #a new olig gets added four times for each element in the list
                newSet.append(olig + n)
        return generateExhaustiveOligomerList(oligomerSize, newSet)
    else:
        return startingSet


'''Used to color the oligomer Counts in grey scale.  Each dictionary represents one line'''


def oligCountToColorSpace(counts, orderedWords):
    if hasDepth(counts):
        return map(lambda x: oligCountToColorSpace(x, orderedWords), counts)
    pixels = []
    for word in orderedWords:
        grey = counts.get(word, 0.0)
        pixels.append(grey)
        pixels.append(grey)
        pixels.append(grey)
    return pixels


'''Returns the reverse complementary sequence.  This is the sequence as it would be read on the
side of the DNA strand (double helix).'''


def reverseComplement(originalSequence):
    complement = {'A': 'T', 'C': 'G', 'G': 'C', 'T': 'A', 'N': 'N'}
    size = len(originalSequence)
    rc = ''
    for x in range(size - 1, -1, -1):#stops at 0
        rc += complement.get(originalSequence[x], 'N')
    return rc


'''Final step for Nucleotide Display that transforms normalized counts into a list of colors'''


def countListToColorSpace(countList, colorPalette, scale):
    if hasDepth(countList):#this recurses until we're left with a single dictionary
        return [countListToColorSpace(x, colorPalette, scale) for x in countList if x != []]
    if not isinstance(countList, dict):
        return []
    colorMapping = colorPalettes[colorPalette]
    colorContributions = []
    basePercentage = 17 - (17 * (1 / (sqrt((scale + 7) / 8.0))))
    '''basePercentage is the percentage share considered not statistically significant which approaches 20% as scale approaches infinity.
    This equation was determined by eyeing different graphs on a graphing calculator and looking for one with the desired properties.'''
    expectedValue = basePercentage / 100.0 #this should work out to be the number of hits that are expected given the scale
    oldMax = max(countList.values())
    for key in countList:
        countList[key] = max(0, countList[key] - expectedValue)
    newMax = max(countList.values())
    for character, Magnitude in countList.items():#per entry in dictionary
        multiplier = Magnitude / (1.0 - expectedValue)
        try:
            currentColor = colorMapping[character]
        except KeyError:
            currentColor = colorMapping['N']
        colorContributions.append(map(lambda c: c * multiplier, currentColor))  # scales color amount by magnitude for each channel
    resultingColor = map(sum, zip(*colorContributions))

    for i in range(len(resultingColor)):
        resultingColor[i] = resultingColor[i] * oldMax / float(newMax)
    return tuple(resultingColor)


def composedOfNs(countDict):
    if isinstance(countDict, dict):
        total = sum(countDict.values())
        return countDict['N'] == total
    else:
        colorMapping = colorPalettes['Classic']
        return countDict == colorMapping['N']


'''Generic normalization reduces any number to a floating point between 0.0 and 1.0'''


def normalize(value, minimum, maximum):
    value = min(maximum, max(minimum, value))
    return (value - minimum) / float(maximum - minimum)


'''ReferencePoint is the number that all elements are divided by.  This defaults to the sum of dictionary 
elements if not defined.  ReferencePoint can also be an evaluation function that returns a single number
when given the values of 'listing' as an argument.'''


def normalizeDictionary(listing, referencePoint=0):
    if hasDepth(listing):#this recurses until we're left with a single dictionary
        return map(lambda x: normalizeDictionary(x, referencePoint), listing)

    if len(listing) == 0:
        return listing
    if referencePoint == 0:
        referencePoint = sum(listing.values())
    elif callable(referencePoint):
        referencePoint = referencePoint(listing.values())
    if referencePoint != 0 and referencePoint > 0.00000000001:
        for key, value in listing.items():
            listing[key] = value * 1.0 / referencePoint
    return listing


def countNucleotides(seq, oligomerSize=1):
    if hasDepth(seq):
        return [countNucleotides(x, oligomerSize) for x in seq if x != '' and x != [] and x != {}]
    if not seq:
        return {}
    counts = {'A': 0, 'C': 0, 'G': 0, 'T': 0, 'N': 0}
    if oligomerSize == 1:  # optimized for Nucleotide Display
        for c in seq:
            try:
                counts[c] = 1 + counts[c]  # counts.get(c,0) #defaults to 0
            except:
                pass
    else:
        for endIndex in range(oligomerSize, len(seq) + 1, 1):
            c = seq[endIndex - oligomerSize: endIndex]
            counts[c] = 1 + counts.get(c, 0) #defaults to 0
    return counts


'''Returns a list of dictionaries that show the counts per grouping (usually scale).
    Doing this adds one layer of depth to seq'''


def chunkUpList(seq, chunkSize, overlap=0):
    if hasDepth(seq):
        return map(lambda x: chunkUpList(x, chunkSize, overlap), seq)
    if chunkSize == 0:
        return seq
    height = int(math.ceil(len(seq) / float(chunkSize)))
    #    if height == 0: return []
    resultVector = [seq[chunk * chunkSize: (chunk + 1) * chunkSize + overlap] for chunk in range(height)]
    return resultVector



def sequenceToColors(seq, colorPalette):
    """Optimized function for Nucleotide to color mapping at scale 1"""
    if hasDepth(seq):
        return map(lambda x: sequenceToColors(x, colorPalette), seq)
    pixels = []
    colorMapping = colorPalettes[colorPalette]
    for c in seq:
        try:
            pixels.append(colorMapping[c])
        except KeyError:
            pixels.append(colorMapping['N'])
    return pixels


'''Take two sequences '''


def calculatePerCharacterMatch(A, B):
#    if hasDepth(Aseq) and hasDepth(Bseq):
#        return map(lambda x: calculatePerCharacterMatch(A, B),... 
    mask = []
    for val in zip(A, B):
        mask.append(val[0] == val[1])
    return mask


def average(values, start=0, length=-1):
    if length == -1:
        length = len(values)
        assert length > 0
        return float(sum(values)) / length
    assert length > 0
    totalSum = 0
    for index in range(start, start + length):
        totalSum += values[index]
    return float(totalSum) / length


'''Slower correlate, but runs on all systems'''


def pythonCorrelate(x, y):
    n = len(x)
    avg_x = average(x)
    avg_y = average(y)
    diffprod = 0.0
    xdiff2 = 0.0
    ydiff2 = 0.0
    for idx in range(n):
        xdiff = float(x[idx]) - avg_x
        ydiff = float(y[idx]) - avg_y
        diffprod += xdiff * ydiff
        xdiff2 += xdiff * xdiff
        ydiff2 += ydiff * ydiff
    backup = math.sqrt(1 - (1 / n)) #if we have 0 instances of a color it will be / 0  div0
    if (xdiff2 == 0.0):
        xdiff2 = backup
    if (ydiff2 == 0.0):
        ydiff2 = backup
    base = math.sqrt(xdiff2 * ydiff2)
    return diffprod / base


'''Pearson correlation coefficient between signals x and y.'''


def pearsonCorrelation(x, y):
    assert len(x) == len(y), (len(x), " vs. ", len(y))
    n = len(x)
    assert n > 0, "Array is empty"
    assert isinstance(x[0], Number), x[0]

    if usingCcode:
        arrX = (ctypes.c_double * len(x))(*x)
        arrY = (ctypes.c_double * len(y))(*y)

        skittleUtils.Correlate.restype = ctypes.c_double
        temp = skittleUtils.Correlate(arrX, arrY, n)
        return temp
    else:
        return pythonCorrelate(x, y)


'''Pearson correlation coefficient between signals x and y.
Thanks to http://stackoverflow.com/users/34935/dfrankow for the definition'''


def fastPearsonCorrelation(data, beginA, beginB, n):
    avg_x = 63.0#average(data, beginA, n)
    avg_y = 63.0#average(data, beginB, n)
    diffprod = 0.0
    xdiff2 = 0.0
    ydiff2 = 0.0
    for idx in range(n):
        xdiff = float(data[idx + beginA]) - avg_x
        ydiff = float(data[idx + beginB]) - avg_y
        diffprod += xdiff * ydiff
        xdiff2 += xdiff * xdiff
        ydiff2 += ydiff * ydiff
        #    backup = .000001 #if we have 0 instances of a color it will be / 0  div0
    if (xdiff2 == 0):
        xdiff2 = 1
    if (ydiff2 == 0):
        ydiff2 = 1
    base = math.sqrt(xdiff2 * ydiff2)
    return diffprod / base


'''Calculates the Pearson Correlation Coefficient for a two spots on the same sequence "floatList".
It ensures that both strings are of the same length and creates substrings for calculation. '''


def correlate(floatList, beginA, beginB, comparisonLength):
    #manipulate signal strings before passing to correlation
    lengthA = min(comparisonLength, len(floatList) - beginA)
    lengthB = min(comparisonLength, len(floatList) - beginB)
    if lengthA == lengthB:
        if lengthA != 0:
            return fastPearsonCorrelation(floatList, beginA, beginB, comparisonLength)
        else:
            return 0
    else:
        return None


'''Calculates the Pearson Correlation Coefficient for a two spots on the same sequence "floatList".
It ensures that both strings are of the same length and creates substrings for calculation. '''


def slowCorrelate(floatList, beginA, beginB, comparisonLength):
    #manipulate signal strings before passing to correlation
    A = floatList[beginA: beginA + comparisonLength]
    B = floatList[beginB: beginB + comparisonLength]
    if len(A) == len(B):
        if len(A) != 0:
        #            numpy.correlate(A, B, mode='same')
            return pearsonCorrelation(A, B)
        else:
            return 0
    else:
        return None


def countMatches(sequence, beginA, beginB, lineSize):
    matches = 0
    for index in range(lineSize):
        if sequence[beginA + index] == sequence[beginB + index]:
            matches += 1
    return float(matches) / lineSize


def oldRepeatMap(state, threeMerState):
    assert isinstance(threeMerState, ThreeMerDetectorState)
    freq = []
    lineSize = state.nucleotidesPerLine()
    for h in range(threeMerState.height(state, state.seq)):
        freq.append([0.0] * (threeMerState.samples * 3 + 1))
        offset = h * lineSize
        for w in range(1, len(freq[h])):
            freq[h][w] = countMatches(state.seq, offset, offset + w, lineSize)
    return freq


def sensitiveTestForSpecificFrequency(floatList, frequency=3, numberOfSamples=20.0):
    """This method takes a series of floating point numbers.  It checks each multiple of "frequency" and determines
    if the sum of samples at frequency are greater than the background level.  It then returns the frequency score.
    If there is no difference, this number will be 0.0.  This score is not currently normalized. This method is used
    to find the 3-periodicity bias found in most protein coding sequences."""
    if hasDepth(floatList):
        return map(lambda x: sensitiveTestForSpecificFrequency(x, frequency, numberOfSamples), floatList)
    assert isinstance(frequency, int), "Please use an integer offset frequency."
    reach = numberOfSamples * frequency
    mask = [] #float
    for i in range(reach + 1):#create mask:
        if (i % frequency == 0):
            mask.append(1.0)
        else:
            mask.append(-1 * (1 / float(frequency - 1)))
    mask[0] = 0.0
    score = 0.0
    for x in range(1, len(mask)):#start at index 1 because repeatMap at offset 0 is undefined
        if floatList[x] is not None:
            score += (mask[x] * floatList[x]) #/ float(numberOfSamples)
    return score

