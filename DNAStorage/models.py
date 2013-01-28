from django.db import models

class Specimen(models.Model):
    Species = models.CharField(max_length=50)
    Genus = models.CharField(max_length=50)
    Class = models.CharField(max_length=50)
    Kingdom = models.CharField(max_length=50)
    GenomeLength = models.IntegerField()
    Source = models.CharField(max_length=255, null=True)
    ExtendedName = models.CharField(max_length=255, null=True)
    Description = models.TextField(null=True)

class Chromosome(models.Model):
    Specimen = models.ForeignKey(Specimen)
    Chromosome = models.CharField(max_length=50)
    Length = models.IntegerField()
    
class ChromosomeChunkFiles(models.Model):
    Chromosome = models.ForeignKey(Chromosome)
    Start = models.IntegerField()
    IsInRamDisk = models.BooleanField(default=False)

class ImageFiles(models.Model):
    Chromosome = models.ForeignKey(Chromosome)
    GraphType = models.CharField(max_length=1)
    Start = models.IntegerField()
    Scale = models.IntegerField(null=True)
    CharactersPerLine = models.IntegerField(null=True)
    IsInRamDisk = models.BooleanField(default=False)
