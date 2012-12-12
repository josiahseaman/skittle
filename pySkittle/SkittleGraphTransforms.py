'''
Created on Nov 29, 2012
@author: Josiah Seaman 
'''
#import numpy
import math
from numbers import Number
from SkittleStatePackets import *
from PixelLogic import colorPalettes


def hasDepth(listLike):
    try:
        return len(listLike) > 0 and not isinstance(listLike, (str,dict)) and hasattr(listLike[0], "__getitem__")
    except:
        return False
    
'''Returns the reverse complementary sequence.  This is the sequence as it would be read on the
side of the DNA strand (double helix).'''
def reverseComplement(originalSequence):
    complement = {'A':'T', 'C':'G', 'G':'C', 'T':'A', 'N':'N'}
    size = len(originalSequence)
    rc = '' 
    for x in range(size-1, -1, -1):#stops at 0
        rc += complement.get(originalSequence[x], 'N')
    return rc

'''Final step for Nucleotide Display that transforms normalized counts into a list of colors'''
def countListToColorSpace(countList, colorPalette):
    if hasDepth(countList):#this recurses until we're left with a single dictionary
        return map(lambda x: countListToColorSpace(x, colorPalette), countList)
    
    colorMapping = colorPalettes[colorPalette]
    resultingColor = [0, 0, 0]
    for character, magnitude in countList.items():#per entry in dictionary
        colorContribution = map(lambda c: c * magnitude, colorMapping[character]) #scales color amount by magnitude for each channel
        resultingColor =  map(sum, zip(colorContribution, resultingColor))
    return resultingColor

def normalizeDictionary(listing):
    if hasDepth(listing):#this recurses until we're left with a single dictionary
        return map(lambda x: normalizeDictionary(x), listing)
    
    if len(listing) == 0: return listing
    total = reduce(lambda Sum, val: Sum+val, listing.values(), 0)
    for key, value in listing.items():
        listing[key] = value*1.0 / total
    return listing

def countNucleotides(seq):
    if hasDepth(seq):
        return map(lambda x: countNucleotides(x), seq)
    counts = {}
    for c in seq:
        counts[c] = 1 + counts.get(c,0) #defaults to 0 
    return counts

'''Returns a list of dictionaries that show the counts per grouping (usually scale).
    Doing this adds one layer of depth to seq'''
def chunkUpList(seq, chunkSize):
    if hasDepth(seq):
        return map(lambda x: chunkUpList(x, chunkSize), seq)
    resultVector = []
    chunk = 0
    while chunk * chunkSize < len(seq):
        resultVector.append(seq[chunk*chunkSize : (chunk+1)*chunkSize])
        chunk += 1
    return resultVector

'''Deprecated.  Nucleotide Display uses normalized counts now''' 
def colorCompress(pixels, scale):
    if hasDepth(pixels):
        return map(lambda x: colorCompress(x, scale), pixels)
    compressed = []
    for i in range(0, len(pixels)- scale, scale):
        r= 0; g = 0; b = 0
        for s in range(scale):
            r += pixels[i+s][0]
            g += pixels[i+s][1]
            b += pixels[i+s][2]
        compressed.append((r / scale, g/scale, b/scale))
    return compressed

'''Optimized function for Nucleotide to color mapping at scale 1'''        
def sequenceToColors(seq, colorPalette):
    if hasDepth(seq):
        return map(lambda x: sequenceToColors(x, colorPalette), seq)
    pixels = []
    colorMapping = colorPalettes[colorPalette]
    for c in seq:
        pixels.append( colorMapping[c] )
    return pixels

'''Take two sequences '''
def calculatePerCharacterMatch(A, B):
#    if hasDepth(Aseq) and hasDepth(Bseq):
#        return map(lambda x: calculatePerCharacterMatch(A, B),... 
    mask = []
    for i, val in enumerate(zip(A,B)):
        mask.append(val[0] == val[1])
    return mask

def average(values):
    assert len(values) > 0
    return float(sum(values)) / len(values)

'''Pearson correlation coefficient between signals x and y.
Thanks to http://stackoverflow.com/users/34935/dfrankow for the definition'''
def __pearsonCorrelation__(x, y):
    assert len(x) == len(y), (len(x) , " vs. " , len(y)) 
    n = len(x)
    assert n > 0, "Array is empty"
    assert isinstance(x[0], Number)
    avg_x = average(x)
    avg_y = average(y)
    diffprod = 0
    xdiff2 = 0
    ydiff2 = 0
    for idx in range(n):
        xdiff = x[idx] - avg_x
        ydiff = y[idx] - avg_y
        diffprod += xdiff * ydiff
        xdiff2 += xdiff * xdiff
        ydiff2 += ydiff * ydiff
    backup = math.sqrt(1 - (1/n)) #if we have 0 instances of a color it will be / 0  div0
    if(xdiff2 == 0): xdiff2 = backup
    if(ydiff2 == 0): ydiff2 = backup
    base = math.sqrt(xdiff2 * ydiff2)
    return diffprod / base

'''Calculates the Pearson Correlation Coefficient for a two spots on the same sequence "floatList".
It ensures that both strings are of the same length and creates substrings for calculation. '''
def correlate(floatList, beginA, beginB, comparisonLength):
    #manipulate signal strings before passing to correlation
    A = floatList[beginA: beginA + comparisonLength]
    B = floatList[beginB: beginB + comparisonLength]
    if(len(A) == len(B)):
        return __pearsonCorrelation__(A, B)
    else:
        return None
    
'''This method is simply a convenience proxy for the correlate(floatList) method.  It splits a color into
parts, feeds them through correlate individually, then averages the scores at the end.'''    
def correlateColors(coloredPixels, beginA, beginB, comparisonLength):
    if len(coloredPixels) == 0:
        return None
    if not hasattr(coloredPixels, "__getitem__"):#we didn't actually receive a composite list
        return correlate(coloredPixels, beginA, beginB, comparisonLength)
    resultSum = 0.0
    for part in range(len(coloredPixels[0])):
        currentChannel = map(lambda x : x[part], coloredPixels)
        correlation = correlate(currentChannel, beginA, beginB, comparisonLength)
        if correlation is None:
            return None
        resultSum += correlation
    return resultSum / len(coloredPixels[0])
        
'''Creates a grey scale map of floating point correlation values.  Used by Repeat Map.
Y axis is each display line of the sequence.  X axis is the frequency space starting at offset 0
and proceeding to RepeatMapState.F_width.  When used in Repeat Map, ColoredPixels is 
the color compressed sequence from the Nucleotide Display.'''
def correlationMap( state, repeatMapState, coloredPixels):
    assert isinstance(repeatMapState, RepeatMapState)
    assert isinstance(state, StatePacket)
    pixelsPerSample = state.width / state.scale
    freq = []
    for h in range(repeatMapState.height(state, coloredPixels)):
        freq.append([None]*(repeatMapState.F_width+1))
        offset = h * pixelsPerSample
        for w in range(1, len(freq[h])):#calculate across widths 1:F_width
            coefficient = correlateColors(coloredPixels, offset, offset + w + repeatMapState.F_start, pixelsPerSample)
            if coefficient != None:
                freq[h][w] = .5 * (1.0 + coefficient)
    return freq
          
'''This method takes a series of floating point numbers.  It checks each multiple of "frequency" and determines
if the sum of samples at frequency are greater than the background level.  It then returns the frequency score.
If there is no difference, this number will be 0.0.  This score is not currently normalized. This method is used
to find the 3-periodicity bias found in most protein coding sequences.'''        
def sensitiveTestForSpecificFrequency(floatList, frequency = 3, numberOfSamples = 20):
    if hasDepth(floatList):
        return map(lambda x: sensitiveTestForSpecificFrequency(x, frequency, numberOfSamples), floatList)
    assert isinstance(frequency, int), "Please use an integer offset frequency."
    reach = numberOfSamples * frequency
    mask = [] #float
    for i in range(reach):#create mask:
        if (i % frequency == 0):
            mask.append(1.0)
        else:
            mask.append(-1 * (1/(frequency-1)))

    score = 0.0
    for x in range( min( len(mask), len(floatList))):
        if floatList[x] is not None:
            score += mask[x] * floatList[x]
        #score += min((float)0.5, mask[x] * freq[y][x])//the amount that any position can affect is capped because of tandem repeats with 100% similarity
    return score


if __name__ == '__main__':
    a = [ 1, 2, 3]
    b = [-1,-2,-3]
    
    '''
    counts = countNucleotides('AAAACGCCGTN')
    print counts
    print normalizeDictionary(counts)
    
    print 'Correlation test case'
    sample = [1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1]
    print correlate(sample, 0, 3, len(sample)/2) 
    for size in range(1, len(sample)):
        print correlate(sample, 0, 0, size),
    print
    for start in range(len(sample)-4):
        print correlate(sample, 0, start, 4),
    '''