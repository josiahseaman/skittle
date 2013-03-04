from models import GFF
from django.conf import settings
import shutil, os, os.path, re

#Generate file name for Annotation chunks  
def generateAnnotationChunkName(gff, start):       
    return gff.FileName + "_" + start + ".gff"
    
#Take a json annotation chunk and store it in the correct disk location and create a reference to it in the DB
def StoreAnnotationChunk(gff, chunk):
    return
    annotationChunkFilePath = settings.SKITTLE_TREE_LOC + "Annotations/chunks/" + fastaFile.Specimen.Kingdom + "/" + fastaFile.Specimen.Class + "/" + fastaFile.Specimen.Genus + "/" + fastaFile.Specimen.Species + "/" + fastaFile.Specimen.Name + "/" + fastaFile.Chromosome + "/"
    if not os.path.exists(annotationChunkFilePath):
        os.makedirs(annotationChunkFilePath)
        
    annotationChunkFilePath = annotationChunkFilePath + generateAnnotationChunkName(gff, chunk.start)
    
    chunkFile = open(annotationChunkFilePath, 'w')
    chunkFile.write(chunk)
    chunkFile.close()
    
    chunkDB = AnnotationJsonChunk()
    chunkDB.GFF = gff
    #chunkDB.Chromosome =