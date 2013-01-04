from django.db import models
from SkittleCore.models import StatePacket

# Create your models here.

class ParentState(models.Model):
    session = models.ForeignKey(StatePacket)
    visible = models.BooleanField(default=False)
    class Meta:
        abstract = True
    

class NucleotideDisplayState(ParentState):
    visible = models.BooleanField(default=True)
    
class NucleotideBiasState(ParentState):
    pass
    
class AnnotationDisplayState(ParentState):
    annotationPath = models.URLField()
    
class OligomerUsageState(ParentState):
    oligomerSize = models.IntegerField(default=2)
    
    

'''This class contains all the state information specific exclusively to Repeat Map. 
This is the link and definition of the settings tab for Repeat Map.
I'm dubious about where to put this since GraphTransforms depends on it, but otherwise
it would be included in RepeatMap.py'''
class RepeatMapState(ParentState):
    F_width = models.IntegerField(default=10)
    F_start = models.IntegerField(default=1)
    
    def height(self, state, pixels):
        assert isinstance(state, StatePacket)
        F_height = ((len(pixels)) - (self.F_start-1)*state.scale - self.F_width*state.scale ) / state.width
        F_height = max(0, min(400, F_height) )
        return F_height
    
class HighlighterState(ParentState):
    searchReverseComplement = models.BooleanField(default=True)
    
    def getTargetSequenceEntries(self):
        '''targetSequenceEntries contains a series of SequenceEntries.'''
        return SequenceEntry.objects.filter(ownerGraph = self)
    
    def __str__(self):
        return "Search other strand: "+ str(self.searchReverseComplement) + "   Sequences: "+ str(self.targetSequenceEntries)
        
class SequenceEntry(models.Model):
    ownerGraph = models.ForeignKey(HighlighterState)
    seq = models.CharField(max_length=1000, default='AAAAAAAA')
    minimumPercentage = models.FloatField(default = .8) 
    color = models.CommaSeparatedIntegerField(max_length=3)
    
            