'''
Created on Nov 30, 2012

@author: Josiah
'''

class StatePacket():
    '''
    This is the single global state packet that defines a view state in Skittle.  
    This state packet is equivalent to an URL or a request from the Skittle website.
    '''
    def __init__(self):
        '''Constructor used for test default values.'''
        self.genome = 'hg19'
        self.chromosome = 'chrY'
        self.seq = 'ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT'
        self.colorPalette = 'Classic'
        self.width = 4
        self.scale = 2
        self.start = 3
        self.length = len(self.seq) - (self.start-1)
        self.activeGraphs = {"Nucleotide Display": None, "Repeat Map": RepeatMapState()}
    
'''This class contains all the state information specific exclusively to Repeat Map. 
This is the link and definition of the settings tab for Repeat Map.
I'm dubious about where to put this since GraphTransforms depends on it, but otherwise
it would be included in RepeatMap.py'''
class RepeatMapState():
    def height(self, state, pixels):
        assert isinstance(state, StatePacket)
        F_height = ((len(pixels)) - (self.F_start-1)*state.scale - self.F_width*state.scale ) / state.width
        F_height = max(0, min(400, F_height) )
        return F_height
    
    def __init__(self):
        '''Constructor used for test default values.'''
        self.barWidth = 20
        self.spacerWidth = 2
        self.F_width = 10
        self.F_start = 1
        self.using3merGraph = True
        
        