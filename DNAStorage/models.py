from django.db import models
from django.conf import settings


class Specimen(models.Model):
    Name = models.CharField(max_length=50)
    Species = models.CharField(max_length=50)
    Genus = models.CharField(max_length=50)
    Class = models.CharField(max_length=50)
    Kingdom = models.CharField(max_length=50)
    GenomeLength = models.BigIntegerField(null=True, default=0)
    ExtendedName = models.CharField(max_length=255, null=True, blank=True)
    Source = models.CharField(max_length=255, null=True, blank=True)
    Description = models.TextField(null=True, blank=True)
    DatePublished = models.CharField(max_length=50, null=True, blank=True)
    Thumbnail = models.CharField(max_length=255, null=True, blank=True)

    def __unicode__(self):
        return self.Name


class FastaFiles(models.Model):
    Specimen = models.ForeignKey(Specimen)
    Chromosome = models.CharField(max_length=50)
    Length = models.IntegerField()
    Public = models.BooleanField()
    User = models.ManyToManyField(settings.AUTH_USER_MODEL, null=True)

    def __unicode__(self):
        return self.Chromosome + " (" + self.Specimen.Name + ")"

class FastaChunkFiles(models.Model):
    FastaFile = models.ForeignKey(FastaFiles)
    Start = models.IntegerField()
    IsInRamDisk = models.BooleanField(default=False)


class ImageFiles(models.Model):
    FastaFile = models.ForeignKey(FastaFiles)
    GraphType = models.CharField(max_length=1)
    Start = models.IntegerField()
    Scale = models.IntegerField(null=True)
    CharactersPerLine = models.IntegerField(null=True)
    IsInRamDisk = models.BooleanField(default=False)


class ImportProgress(models.Model):
    Specimen = models.CharField(max_length=255)
    FileName = models.CharField(max_length=255)
    Message = models.CharField(max_length=255, null=True, blank=True)
    IsWorking = models.BooleanField(default=True)
    Success = models.BooleanField(default=False)
    FastaFile = models.OneToOneField(FastaFiles, null=True, default=None)
    User = models.ManyToManyField(settings.AUTH_USER_MODEL)
