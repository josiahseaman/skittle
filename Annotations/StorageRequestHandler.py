from models import GFF, AnnotationJsonChunk
from DNAStorage.models import FastaFiles, Specimen
from DNAStorage.StorageRequestHandler import GetRelatedFastaFile
from django.conf import settings
import shutil, os, os.path, re

#Generate file name for Annotation chunks  
def generateAnnotationChunkName(gff, start):       
    return gff.FileName + "_" + str(start) + ".gff"
    
#Take a json annotation chunk and store it in the correct disk location and create a reference to it in the DB
def StoreAnnotationChunk(gff, chromosome, chunk, start):
    fastaFile = GetRelatedFastaFile(gff.Specimen, chromosome)
    annotationChunkFilePath = settings.SKITTLE_TREE_LOC + "Annotations/chunks/" + fastaFile.Specimen.Kingdom + "/" + fastaFile.Specimen.Class + "/" + fastaFile.Specimen.Genus + "/" + fastaFile.Specimen.Species + "/" + fastaFile.Specimen.Name + "/" + fastaFile.Chromosome + "/"
    if not os.path.exists(annotationChunkFilePath):
        os.makedirs(annotationChunkFilePath)
        
    annotationChunkFilePath = annotationChunkFilePath + generateAnnotationChunkName(gff, start)
    
    chunkFile = open(annotationChunkFilePath, 'w')
    chunkFile.write(chunk)
    chunkFile.close()
       
    chunkDB, created = AnnotationJsonChunk.objects.get_or_create(GFF = gff, Chromosome = chromosome, Start = start, IsInRamDisk = False)