'''
Created on Nov 29, 2012
@author: Josiah Seaman
'''
import NucleotideDisplay
from SkittleGraphTransforms import correlate 

class RepeatMap():#object
    '''
    classdocs
    '''
    def height(self, pixels, width, scale):
        
        F_height = ((len(pixels)) - (self.F_start-1)*scale - self.F_width*scale ) / width
        F_height = max(0, min(400, F_height) )
        return F_height
        
    def correlationMap(self, pixels, width, scale):
        pixelsPerSample = width / scale
        freq = []
        for h in range(self.height(pixels, width, scale)):
            freq.append([None]*(self.F_width+1))
            offset = h * pixelsPerSample
            for w in range(1, len(freq[h])):#calculate across widths 1:F_width
                freq[h][w] = .5 * (1.0 + correlate(pixels, offset, offset + w + self.F_start, pixelsPerSample))
        return freq
      
    
    def calculateOutputPixels(self, seq, width, start, scale, length):
        pixels = NucleotideDisplay.calculateOutputPixels(seq, width, start, scale, length)
        scores = self.correlationMap(pixels, width, scale)
        return scores

    def __init__(self):
        ''' Constructor '''
        self.barWidth = 20
        self.spacerWidth = 2
        self.F_width = 10
        self.F_start = 1
        self.using3merGraph = True
        
        
if __name__ == '__main__':
    print 'Repeat Map test case'
    RM = RepeatMap()
    seq = 'ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT'
    print RM.calculateOutputPixels(seq, 2, 1, 1, len(seq))
    
    