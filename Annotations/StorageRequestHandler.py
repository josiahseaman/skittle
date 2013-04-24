import os
import re
import json

from django.conf import settings

from models import GFF, AnnotationJsonChunk
from DNAStorage.StorageRequestHandler import GetRelatedFastaFile
import import_snp# import createAnnotationsFromCompact


#Generate file name for Annotation chunks  
def generateAnnotationChunkName(gff, start):
    return gff.FileName + "_" + str(start) + ".gff"

#Parse which chromosome the specific annotation is associated with
def ParseChromosomeName(validChromosomes, seqname):
    possibleMatches = []

    for chromosome in validChromosomes:
        if seqname in chromosome or chromosome in seqname:
            possibleMatches += [chromosome]

    if len(possibleMatches) == 1:
        #Grab fastachunk for one and only match
        return possibleMatches[0]
    else:
        #Look at possible matches and try to guess at the best one
        #First, why don't we remove all non-numbers from both comparisons
        for possible in possibleMatches:
            if re.sub("[^0-9]", "", possible) == re.sub("[^0-9]", "", seqname):
                return possible
        return None


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

    chunkDB, created = AnnotationJsonChunk.objects.get_or_create(GFF=gff, Chromosome=chromosome, Start=start,
                                                                 IsInRamDisk=False)


def GetAnnotationsList(specimen):
    annotationsJson = []

    annotations = GFF.objects.filter(Specimen__Name=specimen)
    if annotations:
        for gff in annotations:
            temp = {"Specimen": gff.Specimen.Name, "GFFVersion": gff.GFFVersion, "SourceVersion": gff.SourceVersion,
                    "Date": gff.Date, "Type": gff.Type, "DNA": gff.DNA, "RNA": gff.RNA, "Protein": gff.Protein,
                    "SequenceRegion": gff.SequenceRegion, "FileName": gff.FileName}
            annotationsJson.append(temp)
        return annotationsJson
    else:
        return None


def GetAnnotationsChunk(specimen, chromosome, start, annotations=None):
    print "Requested chunk", specimen, chromosome, start, annotations
    annotationJsonChunk = []
    if annotations:
        #Go through each given gff file
        for gff in annotations:
            temp = AnnotationJsonChunk.objects.filter(GFF__Specimen__Name=specimen, Chromosome=chromosome, Start=start,
                                                      GFF__FileName=gff)[:1]
            if temp:
                annotationJsonChunk.append(temp[0])
    else:
        #Grab all gff files
        temp = AnnotationJsonChunk.objects.filter(GFF__Specimen__Name=specimen, Chromosome=chromosome, Start=start)
        if temp:
            for annotation in temp:
                annotationJsonChunk.append(annotation)
    if specimen == "hg19":
        annotationJsonChunk.append(import_snp.createAnnotationsFromCompact('23andMe_demo', chromosome, start))

    if len(annotationJsonChunk) >= 1:
        contents = "{"
        for annotation in annotationJsonChunk:
            if isinstance(annotation, AnnotationJsonChunk):
                gff = annotation.GFF
                fastaFile = GetRelatedFastaFile(gff.Specimen, chromosome)

                annotationChunkFilePath = settings.SKITTLE_TREE_LOC + "Annotations/chunks/" + fastaFile.Specimen.Kingdom + "/" + fastaFile.Specimen.Class + "/" + fastaFile.Specimen.Genus + "/" + fastaFile.Specimen.Species + "/" + fastaFile.Specimen.Name + "/" + fastaFile.Chromosome + "/" + generateAnnotationChunkName(
                    gff, start)
                chunkFile = open(annotationChunkFilePath, 'r')

                read = chunkFile.read()
                chunkFile.close()
                read = read[1:-1] + ","

                contents = contents + read
            else:
                contents = contents + annotation[1:-1] + ","

        contents = contents[:-1] + "}"
        return contents
    else:
        return None