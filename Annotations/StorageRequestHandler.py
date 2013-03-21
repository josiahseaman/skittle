from models import GFF, AnnotationJsonChunk
from DNAStorage.models import FastaFiles, Specimen
from DNAStorage.StorageRequestHandler import GetRelatedFastaFile
from django.conf import settings
import shutil, os, os.path, re
import json
import import_snp

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
    chunkFile.write(json.dumps(chunk))
    chunkFile.close()
       
    chunkDB, created = AnnotationJsonChunk.objects.get_or_create(GFF = gff, Chromosome = chromosome, Start = start, IsInRamDisk = False)
    
def GetAnnotationsChunk(specimen, chromosome, start, annotations = None):
    annotationJsonChunk = list()
    if annotations:
        #Go through each given gff file
        for gff in annotations:
            if gff == 'SNP':
                annotationJsonChunk.append(import_snp.createAnnotationsFromCompact('23andMe_demo', chromosome, start))
            else:
                temp = AnnotationJsonChunk.objects.filter(GFF__Specimen__Name = specimen, Chromosome = chromosome, Start = start, GFF__FileName = gff)[:1]
                if temp:
                    annotationJsonChunk.append(temp[0])
    else:
        #Grab all gff files
        temp = AnnotationJsonChunk.objects.filter(GFF__Specimen__Name = specimen, Chromosome = chromosome, Start = start)
        if temp:
            for annotation in temp:
                annotationJsonChunk.append(annotation)
    
    if len(annotationJsonChunk) >= 1:
        contents = "{"
        for annotation in annotationJsonChunk:
            gff = annotation.GFF
            fastaFile = GetRelatedFastaFile(gff.Specimen, chromosome)
            
            annotationChunkFilePath = settings.SKITTLE_TREE_LOC + "Annotations/chunks/" + fastaFile.Specimen.Kingdom + "/" + fastaFile.Specimen.Class + "/" + fastaFile.Specimen.Genus + "/" + fastaFile.Specimen.Species + "/" + fastaFile.Specimen.Name + "/" + fastaFile.Chromosome + "/" + generateAnnotationChunkName(gff, start)
            chunkFile = open(annotationChunkFilePath, 'r')
            
            read = chunkFile.read()
            chunkFile.close()
            read = read[1:-1] + ","
            
            contents = contents + read
        
        contents = contents[:-1] + "}"
        return contents
    else:
        return None