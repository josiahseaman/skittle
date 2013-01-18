'''
Created on Nov 30, 2012

@author: Josiah
'''
from SkittleTree import settings
from django.db import models
import Graphs.models   #import ParentState


'''
This is the single global state packet that defines a view state in Skittle.  
This state packet is equivalent to an URL or a request from the Skittle website.
'''
class RequestPacket(models.Model):
    #TODO: user = models.ForeignKey(User)
    #TODO: session
    specimen = models.CharField(max_length=200, default='hg19')
    chromosome = models.CharField(max_length=200, default='chrY-sample')
    '''It is debatable whether or not the sequence should be stored in the state
    variable since it is only referenced at the first level operation.  Past the first
    step, the input sequence is more likely to be a floatList produced by the previous
    operation.'''
    seq = models.TextField(default=None, Null=True)
    colorPalette = models.CharField(max_length=50, 
                                    choices=[("COLORBLINDSAFE", "Spring"),
                                    ("BETTERCBSAFE", "Summer"),
                                    ("DARK", "DARK"),
                                    ("DRUMS", "DRUMS"),
                                    ("BLUES", "BLUES"),
                                    ("REDS", "REDS"),
                                    ('Classic','Classic')],
                                    default='Classic')
    width = models.IntegerField(default=None, Null=True)
    scale = models.IntegerField(default=None, Null=True)
    '''Internally, start begins at 0.  Biologists count indices starting at 1, so this number 
    is 1 less than the number displayed on the website.  This also means that you should print
    index+1 whenever you are writing user readable output.'''
    start = models.IntegerField(default=None, Null=True)
    length = models.IntegerField(default=65536)
    requestedGraph = models.CharField(max_length=40, default=None, Null=True)

    def getActiveGraphSettings(self):
        return Graphs.models.ParentState.objects.filter(session = self, visible = True)
    
    '''Derived value height may need to be further reduced for functions that must scan ahead.'''
    def height(self):
        return self.length / self.width
    
    def characterPerLine(self):
        return self.width * self.scale
   
class StatePacket(RequestPacket): 
    #TODO: should I restate the models.Charfield(max_length... or just set specimen = 'hg19'?
    specimen = models.CharField(max_length=200, default='hg19')
    chromosome = models.CharField(max_length=200, default='chrY-sample')
    width = models.IntegerField(default= 200)
    scale = models.IntegerField(default=1)
    start = models.IntegerField(default=1)
    length = models.IntegerField(default=65536)
    requestedGraph = models.CharField(max_length=40, default='n')
    
