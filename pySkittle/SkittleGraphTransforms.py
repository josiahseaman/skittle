'''
Created on Nov 29, 2012

@author: Josiah Seaman 
'''
#import numpy
import math


colorMapping = {'A':(0, 0, 0),#BLACK - Adenine
    'C':(255, 0, 0),#RED - Cytosine
    'G':(0, 255, 0),#GREEN - Guanine
    'T':(0, 0, 255),#BLUE - Thymine
    'N':( 200, 200, 200)#not sequenced
    }


def reverseComplement(original):
    complement = {'A':'T', 'C':'G', 'G':'C', 'T':'A', 'N':'N'}
    size = len(original)
    rc = str(size, 'N')
    for x in range(size):
        rc[x] = complement(original[size-x-1])
    return rc

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
        

def sequenceToColors(seq):
    pixels = []
    for c in seq:
        pixels.append( colorMapping[c] )
    return pixels

def average(values):
    assert len(values) > 0
    return float(sum(values)) / len(values)

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
#
#'''correlation will be a value between -1 and 1 representing how closely related 2 sequences are'''
#def correlationCoefficient(signalA, signalB):
#    N = len(signalA)
#    
#    ARedsum = 0
#    BRedsum = 0
#    ASquaredRed = 0   #this is Aij^2
#    BSquaredRed = 0   #this is Bij^2
#    ABRed = 0   #this is A[]*B[]
#
#    for k in range(N):
#        Avalue = signalA[k]
#        Bvalue = signalB[k]
#        ARedsum += Avalue
#        BRedsum += Bvalue
#        ASquaredRed += (Avalue*Avalue)
#        BSquaredRed += (Bvalue*Bvalue)
#        ABRed += (Avalue * Bvalue)
#
#    AbarRed = ARedsum / N
#    BbarRed = BRedsum / N
#
#    numerator_R = ABRed   - BbarRed   * ARedsum   - AbarRed * BRedsum     + AbarRed   * BbarRed   * N
#
#    denom_R1 = (math.sqrt(ASquaredRed   - ((ARedsum   * ARedsum)  /N)))
#    denom_R2 = (math.sqrt(BSquaredRed   - ((BRedsum   * BRedsum)  /N)))
#
#    backup = math.sqrt(1 - (1/N)) #if we have 0 instances of a color it will be / 0  div0
#    if(denom_R1 == 0): denom_R1 = backup; print 'div0'
#    if(denom_R2 == 0): denom_R2 = backup; print ' div0'
#
#    answer_R = numerator_R / (denom_R1 * denom_R2)
#    return answer_R

def correlate(greyPixels, beginA, beginB, comparisonLength):
    #manipulate signal strings before passing to numpy
    A = greyPixels[beginA: beginA + comparisonLength]
    B = greyPixels[beginB: beginB + comparisonLength]
    if(len(A) == len(B)):
        return pearson_def(A, B)
    else:
        return None
#    return numpy.correlate(A, B)#using this, you need to normalize the arrays beforehand
    
def correlateColors(coloredPixels, beginA, beginB, comparisonLength):
    redChannel = map(lambda x : x[0], coloredPixels)
    greenChannel = map(lambda x: x[1], coloredPixels)
    blueChannel = map(lambda x: x[2], coloredPixels)
    Rdot = correlate(redChannel, beginA, beginB, comparisonLength)
    Gdot = correlate(greenChannel, beginA, beginB, comparisonLength)
    Bdot = correlate(blueChannel, beginA, beginB, comparisonLength)
    if Rdot != None and Gdot != None and Bdot != None:
        average = (Rdot + Gdot + Bdot) / 3.0
        return average
    else:
        return None

        
if __name__ == '__main__':
    print 'Correlation test case'
    sample = [1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1]
    print correlate(sample, 0, 3, len(sample)/2) 
    for size in range(1, len(sample)):
        print correlate(sample, 0, 0, size),
    print
    for start in range(len(sample)-4):
        print correlate(sample, 0, start, 4),
    
