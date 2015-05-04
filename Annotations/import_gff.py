import sys
import math

from django.conf import settings

from models import Annotation, GFF
from DNAStorage.StorageRequestHandler import GetRelatedChromosomes, GetSpecimen
from StorageRequestHandler import StoreAnnotationChunk, ParseChromosomeName
from Utilities.SkittleUtils import GetRoundedIndex


#Import a GFF for a specific specimen
def ImportGFF(specimen, file):
    fileName = file.split('/')
    fileName = fileName[-1].split('.')[0]

    specimenObject = GetSpecimen(specimen)

    gff, created = GFF.objects.get_or_create(Specimen=specimenObject, FileName=fileName, Public=True, User=None)
    print gff.FileName
    #Get the specimen this annotation file is for
    gff.Specimen = GetSpecimen(specimen)
    #Set default version type
    gff.GFFVersion = 2

    #Open file and check for version type
    #TODO: Grab GFF Info and store in database
    annotationFile = open(file, 'r')
    counter = 0
    for line in annotationFile.readlines():
        counter += 1

        if line.startswith("##"):
            if "gff-version" in line:
                temp = line.split(' ')
                gff.GFFVersion = temp[1]

        if counter == 10:
            break
    annotationFile.close()
    gff.save()

    #Grab a list of chromosomes related to this specimen
    validChromosomes = GetRelatedChromosomes(gff.Specimen)

    annotations = {}

    #In GFF format, each line is an annotation. 
    #Read in each annotation and split at tabs (as per gff spec)
    #Then store as needed into an Annotation object which will be pushed onto the annotations stack
    #TODO: Bust out version reading into methods
    if gff.GFFVersion == 2:
        print "BEGINNING READING OF VERSION 2 FILE...\n"
        annotationFile = open(file, 'r')
        counter = 0
        for line in annotationFile.readlines():
            counter += 1
            if not line.startswith('#'):
                elements = line.split('\t')

                annotation = Annotation()
                #TODO: Handle when values come back null from not finding a matching chromosome!
                annotation.Specimen = specimen
                annotation.Chromosome = ParseChromosomeName(validChromosomes,
                                                            elements[0]) #Related validChromosomes, chromosome
                annotation.ID = counter
                annotation.Source = elements[1]
                annotation.Feature = elements[2]
                annotation.Start = elements[3]
                annotation.End = elements[4]

                #Do checking for Score
                if elements[5] == '.':
                    annotation.Score = None
                else:
                    annotation.Score = elements[5]

                #Do checking for Strand
                if elements[6] == '.':
                    annotation.Strand = None
                else:
                    annotation.Strand = elements[6]

                #Do checking for frame
                if elements[7] == '.':
                    annotation.Frame = None
                else:
                    annotation.Frame = elements[7]

                #Check for existence of attribute and possible extra length
                if len(elements) >= 9:
                    annotation.Attribute = elements[8:]
                else:
                    annotation.Attribute = None

                if annotation.Chromosome in annotations:
                    #appent to the current list
                    annotations[annotation.Chromosome].append(annotation)
                else:
                    #Create the chromosome entry in the annotations dictionary
                    annotations[annotation.Chromosome] = []
                    annotations[annotation.Chromosome].append(annotation)

                if counter % 10000 == 0:
                    sys.stdout.write('.')

                    #print "RESULTS: ", annotation.Specimen, annotation.Source, annotation.Feature, annotation.Start, annotation.End, annotation.Score, annotation.Strand, annotation.Frame, annotation.Attribute

        print "DONE READING FILE!"
    else:
        print "THIS GFF VERSION IS NOT SUPPORTED: Version", gff.GFFVersion

    print "BEGINNING SORTING LIST..."
    #Sort the list of annotations read in by their start value (this could probably be optimized by using an always ordered list and inserting in order above)
    #annotations = sorted(annotations, key = lambda annotation: annotation.Start)
    for sublist in annotations:
        annotations[sublist] = sorted(annotations[sublist], key=lambda annotation: int(annotation.Start))
    print "DONE SORTING!"

    chunkAndStoreAnnotations(gff, annotations)

#Take a sorted list of annotations and chunk it into json chunks
def chunkAndStoreAnnotations(gff, annotations):
    print "START CHUNKING..."
    for chromosome in annotations:
        if chromosome is None:
            continue

        chunkStart = 1
        chunkEnd = settings.CHUNK_SIZE
        chunk = {gff.FileName: {}}
        active = []

        print chromosome
        index = 0
        while index < len(annotations[chromosome]):
            if int(annotations[chromosome][index].Start) <= chunkEnd:
                chunk[gff.FileName][str(gff.id) + "-" + str(annotations[chromosome][index].ID)] = {
                    "Source": annotations[chromosome][index].Source, "Feature": annotations[chromosome][index].Feature,
                    "Start": annotations[chromosome][index].Start, "End": annotations[chromosome][index].End,
                    "Score": annotations[chromosome][index].Score, "Strand": annotations[chromosome][index].Strand,
                    "Frame": annotations[chromosome][index].Frame,
                    "Attribute": annotations[chromosome][index].Attribute}

                if int(annotations[chromosome][index].End) > chunkEnd:
                    active.append(annotations[chromosome][index])
            else:
                parseActiveList(gff, chromosome, annotations, index, active, chunk, chunkStart, chunkEnd)

                chunk = {gff.FileName: {}}
                chunkStart = GetRoundedIndex(annotations[chromosome][index].Start)
                chunkEnd = chunkStart + settings.CHUNK_SIZE - 1
                chunk[gff.FileName][str(gff.id) + "-" + str(annotations[chromosome][index].ID)] = {
                    "Source": annotations[chromosome][index].Source, "Feature": annotations[chromosome][index].Feature,
                    "Start": annotations[chromosome][index].Start, "End": annotations[chromosome][index].End,
                    "Score": annotations[chromosome][index].Score, "Strand": annotations[chromosome][index].Strand,
                    "Frame": annotations[chromosome][index].Frame,
                    "Attribute": annotations[chromosome][index].Attribute}

                if int(annotations[chromosome][index].End) > chunkEnd:
                    active.append(annotations[chromosome][index])
            index += 1

        StoreAnnotationChunk(gff, chromosome, chunk, chunkStart)
        parseActiveList(gff, chromosome, annotations, -1, active, chunk, chunkStart, chunkEnd)

    print "DONE CHUNKING!"


def parseActiveList(gff, chromosome, annotations, index, active, chunk, chunkStart, chunkEnd):
    toRemove = []
    for an in active:
        if int(an.Start) < chunkStart:
            chunk[gff.FileName][str(gff.id) + "-" + str(an.ID)] = {"Source": an.Source, "Feature": an.Feature,
                                                                   "Start": an.Start, "End": an.End, "Score": an.Score,
                                                                   "Strand": an.Strand, "Frame": an.Frame,
                                                                   "Attribute": an.Attribute}
        if int(an.End) <= chunkEnd:
            toRemove.append(an)
    for rem in toRemove:
        active.remove(rem)
    StoreAnnotationChunk(gff, chromosome, chunk, chunkStart)
    #GO THROUGH ALL ACTIVE, THEN ADD IN CURRENT!!!
    if len(active) > 0:
        chunk = {gff.FileName: {}}
        chunkStart += settings.CHUNK_SIZE
        chunkEnd = chunkStart + settings.CHUNK_SIZE - 1
        if index != -1:
            while int(annotations[chromosome][index].Start) > int(chunkEnd):
                toRemove = []
                for an in active:
                    chunk[gff.FileName][str(gff.id) + "-" + str(an.ID)] = {"Source": an.Source, "Feature": an.Feature,
                                                                           "Start": an.Start, "End": an.End,
                                                                           "Score": an.Score, "Strand": an.Strand,
                                                                           "Frame": an.Frame, "Attribute": an.Attribute}
                    if int(an.End) <= chunkEnd:
                        toRemove.append(an)
                for rem in toRemove:
                    active.remove(rem)
                StoreAnnotationChunk(gff, chromosome, chunk, chunkStart)
                chunk = {gff.FileName: {}}
                chunkStart += settings.CHUNK_SIZE
                chunkEnd = chunkStart + settings.CHUNK_SIZE - 1
        else:
            while len(active) > 0:
                toRemove = []
                for an in active:
                    chunk[gff.FileName][str(gff.id) + "-" + str(an.ID)] = {"Source": an.Source, "Feature": an.Feature,
                                                                           "Start": an.Start, "End": an.End,
                                                                           "Score": an.Score, "Strand": an.Strand,
                                                                           "Frame": an.Frame, "Attribute": an.Attribute}
                    if int(an.End) <= chunkEnd:
                        toRemove.append(an)
                for rem in toRemove:
                    active.remove(rem)
                StoreAnnotationChunk(gff, chromosome, chunk, chunkStart)
                chunk = {gff.FileName: {}}
                chunkStart += settings.CHUNK_SIZE
                chunkEnd = chunkStart + settings.CHUNK_SIZE - 1