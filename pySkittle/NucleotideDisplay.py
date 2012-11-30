'''
Created on Nov 29, 2012
@author: Josiah
'''
from SkittleGraphTransforms import sequenceToColors
from SkittleGraphTransforms import colorCompress
    
    
def calculateOutputPixels(seq, width, start, scale, length):
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
    print calculateOutputPixels(seq, 10, start, scale, length)
    print "Correct Answer is:"
    print "[(0, 127, 127), (127, 0, 0), (0, 127, 127), (127, 0, 0), (0, 127, 127), (127, 0, 0)]"

    
    
    