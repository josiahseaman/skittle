'''
Created on Nov 29, 2012

@author: Josiah Seaman 
'''

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


def correlate(pixels, beginA, beginB, pixelsPerSample):
    #manipulate signal strings before passing to numpy
    return 0

