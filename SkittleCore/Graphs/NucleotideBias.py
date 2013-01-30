'''
Created on Dec 5, 2012
@author: Josiah
'''
from SkittleCore.models import RequestPacket
from SkittleGraphTransforms import *
from PixelLogic import *
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('b', "Nucleotide Bias", __name__, False, True)
max_bar_width = 20

def calculateBiasBarSizes(state):
    order = ['C', 'G', 'A', 'T', 'N']
    tempWidth = state.nucleotidesPerLine()
    lines = chunkUpList(state.seq, tempWidth )
    countsPerLine = countNucleotides(lines, 1)
    barLengthsPerLine = []
    for h in range( len(countsPerLine) ):#once per line
        bar_sizes = []
        remainder = 0.0
        floating_sum = 0.0
        for key in order:
            barSize = float(countsPerLine[h].get(key,0)) / tempWidth * max_bar_width + .00001  #normalize the size of the bar to display_width
            floating_sum += barSize
            barSize += remainder
            remainder = floating_sum - int(floating_sum + .5)
            tupleT = (key, int(barSize+.5))
            bar_sizes.append( tupleT )
        barLengthsPerLine.append(bar_sizes)
    return barLengthsPerLine



def calculateOutputPixels(state):
    barSizesPerLine = calculateBiasBarSizes(state)
    outputPixels = []
    for line in range(len(barSizesPerLine)):
        barSizes  = map(lambda entry : entry[1], barSizesPerLine[line])
#        print barSizes
        colorSeries = map(lambda entry : getColor(state, entry[0]), barSizesPerLine[line])
        bar = drawJustifiedBar(barSizes, colorSeries, max_bar_width)
#        print bar
        outputPixels.append( bar )
    return outputPixels
    
    
    