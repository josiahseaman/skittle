'''
Created on Nov 29, 2012

@author: Josiah Seaman 
'''
#import numpy
import math

colorPalettes = {
"COLORBLINDSAFE" : 
        {'A': (255, 102, 0),
        'C': (153, 0, 0),
        'G': (51, 255, 51),
        'T': (0, 153, 204),
        'N': ( 200, 200, 200)},#not sequenced

    
"BETTERCBSAFE" : 
        {'A': (0, 204, 204),
        'C': (153, 255, 0),
        'G': (204, 0, 102),
        'T': (255, 102, 0),
        'N': ( 200, 200, 200)},#not sequenced
    
"DARK" : 
        {'A': (152, 147, 173),
        'C': (84, 40, 59),
        'G': (13, 94, 58),
        'T': (40, 75, 163),
        'N': ( 200, 200, 200)},#not sequenced
    
"DRUMS" : 
        {'A': (80, 80, 255),
        'C': (224, 0, 0),
        'G': (0, 192, 0),
        'T': (230, 230, 0),
        'N': ( 200, 200, 200)},#not sequenced
    
"BLUES" : 
        {'A': (141, 0, 74),
        'C': (82, 0, 124),
        'G': (17, 69, 134),
        'T': (14, 112, 118),
        'N': ( 200, 200, 200)},#not sequenced
"REDS" : 
        {'A': (141, 0, 74),
        'C': (159, 0, 0),
        'G': (196, 90, 6),
        'T': (218, 186, 8),
        'N': ( 200, 200, 200)},#not sequenced
    

"Classic" : 
        {'A': (0, 0, 0),
        'C': (255, 0, 0),
        'G': (0, 255, 0),
        'T': (0, 0, 255),
        'N': ( 200, 200, 200)}#not sequenced
    }

def reverseComplement(originalSequence):
    complement = {'A':'T', 'C':'G', 'G':'C', 'T':'A', 'N':'N'}
    size = len(originalSequence)
    rc = str(size, 'N')
    for x in range(size):
        rc[x] = complement(originalSequence[size-x-1])
    return rc

def normalizeDictionary(listing):
    if len(listing) is 0: return listing
    total = 0
    for value in listing.itervalues():
        total +=  value
    for key, value in listing.items():
        listing[key] = value*1.0 / total
    return listing

def countNucleotides(seq):
    counts = {}
    for c in seq:
        counts[c] = 1 + counts.get(c,0) #defaults to 0 
    return counts

def colorCompress(pixels, scale):
    compressed = []
    for i in range(0, len(pixels)- scale, scale):
        r= 0; g = 0; b = 0
        for s in range(scale):
            r += pixels[i+s][0]
            g += pixels[i+s][1]
            b += pixels[i+s][2]
        compressed.append((r / scale, g/scale, b/scale))
    return compressed
        
def sequenceToColors(seq, colorPalette):
    pixels = []
    colorMapping = colorPalettes[colorPalette]
    for c in seq:
        pixels.append( colorMapping[c] )
    return pixels

def average(values):
    assert len(values) > 0
    return float(sum(values)) / len(values)

'''Pearson correlation coefficient between signals x and y.
Thanks to http://stackoverflow.com/users/34935/dfrankow for the definition'''
def pearson_def(x, y):
    assert len(x) == len(y), (len(x) , " vs. " , len(y)) 
    n = len(x)
    assert n > 0, "Array is empty"
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


def correlate(greyPixels, beginA, beginB, comparisonLength):
    #manipulate signal strings before passing to correlation
    A = greyPixels[beginA: beginA + comparisonLength]
    B = greyPixels[beginB: beginB + comparisonLength]
    if(len(A) == len(B)):
        return pearson_def(A, B)
    else:
        return None
    
def correlateColors(coloredPixels, beginA, beginB, comparisonLength):
    if len(coloredPixels) is 0 or len(coloredPixels[0]) is 0:
        return None
    resultSum = 0.0
    for part in range(len(coloredPixels[0])):
        currentChannel = map(lambda x : x[part], coloredPixels)
        correlation = correlate(currentChannel, beginA, beginB, comparisonLength)
        if correlation is None:
            return None
        resultSum += correlation
    return resultSum / len(coloredPixels[0])
        
if __name__ == '__main__':
    counts = countNucleotides('AAAACGCCGTN')
    print counts
    print normalizeDictionary(counts)
    '''
    print 'Correlation test case'
    sample = [1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1]
    print correlate(sample, 0, 3, len(sample)/2) 
    for size in range(1, len(sample)):
        print correlate(sample, 0, 0, size),
    print
    for start in range(len(sample)-4):
        print correlate(sample, 0, start, 4),
    '''
