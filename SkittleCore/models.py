'''
Created on Nov 30, 2012

@author: Josiah
'''
from django.db import models
import Graphs.models   #import ParentState

'''
This is the single global state packet that defines a view state in Skittle.  
This state packet is equivalent to an URL or a request from the Skittle website.
'''
class StatePacket(models.Model):
    #TODO: user = models.ForeignKey(User)
    genome = models.CharField(max_length=200, default='hg19')
    chromosome = models.CharField(max_length=200, default='chrY')
    filePath = models.CharField(max_length=1000, default='testFiles/chrY-sample.fa')
    '''It is debatable whether or not the sequence should be stored in the state
    variable since it is only referenced at the first level operation.  Past the first
    step, the input sequence is more likely to be a floatList produced by the previous
    operation.'''
    seq = models.TextField(default='ACGTAAAACCCCGGGGTTTTACGTACGTACGTACGTACGTACGTACGTACGTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTACGTACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA')
    colorPalette = models.CharField(max_length=50, 
                                    choices=[("COLORBLINDSAFE", "Spring"),
                                    ("BETTERCBSAFE", "Summer"),
                                    ("DARK", "DARK"),
                                    ("DRUMS", "DRUMS"),
                                    ("BLUES", "BLUES"),
                                    ("REDS", "REDS"),
                                    ('Classic','Classic')],
                                    default='Classic')
    width = models.IntegerField(default= 4)
    scale = models.IntegerField(default=2)
    '''Internally, start begins at 0.  Biologists count indices starting at 1, so this number 
    is 1 less than the number displayed on the website.  This also means that you should print
    index+1 whenever you are writing user readable output.'''
    start = models.IntegerField(default=0)
    length = models.IntegerField(default=65536)
    requestedGraph = models.CharField(max_length=40, default='n')

    def calculateFilePath(self):
        self.filePath = self.genome + self.chromosome
        return self.filePath
        
    def getActiveGraphs(self):
        return Graphs.models.ParentState.objects.filter(session = self, visible = True)
    
    '''Derived value height may need to be further reduced for functions that must scan ahead.'''
    def height(self):
        return self.length / self.width
    
    def characterPerLine(self):
        return self.width * self.scale
    
#    def length(self):
#        return len(self.seq) - self.start
    