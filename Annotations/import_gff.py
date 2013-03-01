from django.db import models
from models import Annotation, GFF
from DNAStorage.StorageRequestHandler import GetRelatedChromosomes, GetFastaChunkFile, GetSpecimen, StoreAnnotationChunk
from django.conf import settings

import sys, math, re

#Import a GFF for a specific specimen
def ImportGFF(specimen, file):
    gff = GFF()
    fileName = file.split('/')
    gff.FileName = fileName[-1].split('.')[0]
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
        counter = counter + 1
        
        if line.startswith("##"):
            if "gff-version" in line:
                temp = line.split(' ')
                gff.GFFVersion = temp[1]
        
        if counter == 10:
            break
    annotationFile.close()
    gff.save()
    
    #Grab a list of chromosomes related to this specimen
    validChromosomes = GetRelatedChromosomes(specimen)
    
    annotations = dict()
    
    #In GFF format, each line is an annotation. 
    #Read in each annotation and split at tabs (as per gff spec)
    #Then store as needed into an Annotation object which will be pushed onto the annotations stack
    #TODO: Bust out version reading into methods
    if gff.GFFVersion == 2:
        print "BEGINNING READING OF VERSION 2 FILE...\n"
        annotationFile = open(file, 'r')
        counter = 0
        for line in annotationFile.readlines():
            counter = counter + 1
            if not line.startswith('#'):
                elements = line.split('\t')   

                annotation = Annotation()
                #TODO: Handle when values come back null from not finding a matching chromosome!
                annotation.Specimen = specimen
                annotation.Chromosome = parseChromosomeName(validChromosomes, elements[0]) #Related validChromosomes, chromosome
                annotation.ID = counter
                annotation.Source = elements[1]
                annotation.Feature = elements[2]
                annotation.Start = elements[3]
                annotation.End = elements[4]
        
                #Do checking for Score
                if elements[5] == '.':
                    annotation.Score = 0
                else:
                    annotation.Score = elements[5]
            
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
                    annotations[annotation.Chromosome] = list()
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
        sublist = sorted(sublist, key = lambda annotation: annotation.Start)
    print "DONE SORTING!"
    
    chunkAndStoreAnnotations(gff, annotations)
    
#Parse which chromosome the specific annotation is associated with
def parseChromosomeName(validChromosomes, seqname):
    possibleMatches = list()
    
    for chromosome in validChromosomes: 
        if seqname in chromosome:
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
        
#Take a sorted list of annotations and chunk it into json chunks
def chunkAndStoreAnnotations(gff, annotations):
    print "START CHUNKING..."
    chunkStart = 1
    chunkEnd = settings.CHUNK_SIZE
    jsonStart = "\"" + gff.FileName + "\":{"
    chunk = jsonStart
    
    for chromosome in annotations:
        for annotation in chromosome:
            if int(annotation.Start) <= chunkEnd:
                frame = annotation.Frame or "null"
                if annotation.Attribute:
                    attribute = "\"" + ''.join(annotation.Attribute).replace('\n', '') + "\""
                else:
                    attribute = "null"
                
                chunk += "\"" + str(annotation.ID) + "\":[\"" + annotation.Source + "\",\"" + annotation.Feature + "\"," + str(annotation.Start) + "," + str(annotation.End) + "," + str(annotation.Score) + ",\"" + annotation.Strand + "\"," + str(frame) + "," + attribute + "],"
            else:
                chunk = chunk[:-1] +  "}"
                print chunk
                return
                StoreAnnotationChunk(gff, chunk, annotation.Chromosome)
                chunk = jsonStart
                chunkStart = chunkEnd + 1
                chunkEnd = chunkStart + settings.CHUNK_SIZE - 1
    print "DONE CHUNKING!"
            
    
def getRoundedIndex(index):
    return int(math.floor(int(index) / settings.CHUNK_SIZE) * settings.CHUNK_SIZE) + 1