from django.db import models
from DNAStorage.models import FastaFiles

# Database for storing annotations
class Annotation(models.Model):
    Connection = models.ForeignKey(FastaFiles)
    Source = models.CharField(max_length=255, null=True, blank=True)
    Feature = models.CharField(max_length=50)
    Start = models.BigIntegerField()
    End = models.BigIntegerField()
    Score = models.FloatField(null=True)
    Strand = models.CharField(max_length=1, null=True, blank=True)
    Frame = models.SmallIntegerField(null=True)
    Attribute = models.TextField(null=True, blank=True)
