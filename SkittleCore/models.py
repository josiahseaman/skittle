'''
Created on Nov 30, 2012

@author: Josiah
'''
from django.db import models

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
        self.activeGraphs = {"Nucleotide Display": None, "Repeat Map": 'RepeatMapState()', "Sequence Highlighter":'HighlighterState()'}
    