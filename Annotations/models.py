from django.db import models
from django.conf import settings

from DNAStorage.models import Specimen

# Database for storing imported GFF information
class GFF(models.Model):
    Specimen = models.ForeignKey(Specimen)
    GFFVersion = models.SmallIntegerField(null=True)
    SourceVersion = models.CharField(max_length=255, null=True, blank=True)
    Date = models.DateField(null=True)
    Type = models.CharField(max_length=255, null=True, blank=True)
    DNA = models.TextField(null=True, blank=True)
    RNA = models.TextField(null=True, blank=True)
    Protein = models.TextField(null=True, blank=True)
    SequenceRegion = models.CharField(max_length=255, null=True, blank=True)
    FileName = models.CharField(max_length=255)
    Public = models.BooleanField()
    User = models.ManyToManyField(settings.AUTH_USER_MODEL, null=True)

# Database of all gff chunk files storing json
class AnnotationJsonChunk(models.Model):
    GFF = models.ForeignKey(GFF)
    Chromosome = models.CharField(max_length=25)
    Start = models.BigIntegerField()
    IsInRamDisk = models.BooleanField(default=False)

# Object for storing temporary annotations
class Annotation():
    Specimen = models.CharField(max_length=255)
    Chromosome = models.CharField(max_length=25)
    ID = models.BigIntegerField()
    Source = models.CharField(max_length=255)
    Feature = models.CharField(max_length=50)
    Start = models.BigIntegerField()
    End = models.BigIntegerField()
    Score = models.FloatField(null=True)
    Strand = models.CharField(max_length=1, null=True)
    Frame = models.SmallIntegerField(null=True)
    Attribute = models.TextField(null=True, blank=True)


class SnpIndexInfo(models.Model):
    SnpName = models.CharField(max_length=255) #rs also serves as unique id
    Chromosome = models.CharField(max_length=25) #chromosome
    Start = models.BigIntegerField() #start
    CompactIndex = models.BigIntegerField() #compact index

    def __str__(self):
        return self.SnpName + self.Chromosome + str(self.CompactIndex)

    def __repr__(self):
        return self.__str__()
    
    
   
    
    