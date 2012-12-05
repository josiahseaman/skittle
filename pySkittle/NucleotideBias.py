'''
Created on Dec 5, 2012
@author: Josiah
'''
from SkittleStatePackets import StatePacket
from SkittleGraphTransforms import *

max_bar_width = 40

def calculateOutputPixels(state):
    order = ['C', 'G', 'A', 'T', 'N']
    outputPixels = []
    tempWidth = state.width
    genome = state.seq[state.start : state.start + state.length]
    countsPerLine = countNucleotideGroups(genome, tempWidth )
    for h in range( len(countsPerLine) ):#once per line
        bar_sizes = []
        remainder = 0.0
        floating_sum = 0.0
        for key in order.keys():
            barSize = float(countsPerLine[h][key]) / tempWidth * max_bar_width + .00001  #normalize the size of the bar to display_width
            floating_sum += barSize
            barSize += remainder
            remainder = floating_sum - int(floating_sum + .5)
            bar_sizes.append(int(barSize+.5))
        bar = []#drawJustifiedBar(bar_sizes, max_bar_width, glWidget) # vector<color>
        oldSize = len(outputPixels) 
        outputPixels += bar #TODO check that this does not append as a list
        assert len(outputPixels) == oldSize + len(bar), "appending bar happened incorrectly, insert items individually"
    return


if __name__ == '__main__':
    state = StatePacket()
    print calculateOutputPixels(state)