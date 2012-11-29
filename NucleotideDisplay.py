'''
Created on Nov 29, 2012
@author: Josiah
'''

colorMapping = {'A':(0, 0, 0),#BLACK - Adenine
    'C':(255, 0, 0),#RED - Cytosine
    'G':(0, 255, 0),#GREEN - Guanine
    'T':(0, 0, 255),#BLUE - Thymine
    'N':( 200, 200, 200)#not sequenced
    }


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
    
    
def calculateOutputPixels(seq, start, scale, length):
    seq = seq[start-1 : (start-1) + length] #substrings the relevant part
    pixels = sequenceToColors(seq)
    if scale > 1:
        pixels = colorCompress(pixels, scale)
    return pixels
    
if __name__ == '__main__':
    print 'Nucleotide Display test case'
    seq = 'ACGTACGTACGTACGT'
    scale = 2
    start = 3
    length = len(seq) - (start-1)
    print calculateOutputPixels(seq, start, scale, length)

    
    
    