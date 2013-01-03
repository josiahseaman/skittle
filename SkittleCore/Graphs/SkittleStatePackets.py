'''
Created on Nov 30, 2012

@author: Josiah
'''


'''
This is the single global state packet that defines a view state in Skittle.  
This state packet is equivalent to an URL or a request from the Skittle website.
'''
class StatePacket():
    '''Derived value height may need to be further reduced for functions that must scan ahead.'''
    def height(self):
        return self.length / self.width
    
    def __init__(self):
        '''Constructor used for test default values.'''
        self.genome = 'hg19'
        self.chromosome = 'chrY'
        self.filePath = '../chrY-sample.fa'
        '''It is debatable whether or not the sequence should be stored in the state
        variable since it is only referenced at the first level operation.  Past the first
        step, the input sequence is more likely to be a floatList produced by the previous
        operation.'''
        self.seq = 'ACGTAAAACCCCGGGGTTTTACGTACGTACGTACGTACGTACGTACGTACGTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTACGTACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'
        self.colorPalette = 'Classic'
        self.width = 4
        self.scale = 2
        '''Internally, start begins at 0.  Biologists count indices starting at 1, so this number 
        is 1 less than the number displayed on the website.  This also means that you should print
        index+1 whenever you are writing user readable output.'''
        self.start = 0 
        self.length = len(self.seq) - self.start
        self.activeGraphs = {"Nucleotide Display": None, "Repeat Map": RepeatMapState(), "Sequence Highlighter":HighlighterState()}
    
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
        self.F_width = 10
        self.F_start = 1
        
class SequenceEntry():
    def __init__(self, seq, minimumSimilarity, color):
        self.seq = str(seq)
        self.minimumPercentage = minimumSimilarity
        self.color = color
        
class HighlighterState():
    def __str__(self):
        return "Search other strand: "+ str(self.searchReverseComplement) + "   Sequences: "+ str(self.targetSequenceEntries)
    
    def __init__(self):
        '''targetSequenceEntries contains a series of SequenceEntries.'''
        self.targetSequenceEntries = [SequenceEntry('AAAA', .7, (20, 250, 20) ), SequenceEntry('GGGGG', .7, (250, 20, 20) )]
        self.searchReverseComplement = True
        