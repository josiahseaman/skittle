import os
import re
import shutil

from django.conf import settings

from models import FastaFiles, FastaChunkFiles, Specimen
from StorageRequestHandler import HasFastaFile


#Take a fasta file, split it and sort it into the correct folders
def splitAndSort(file, storageLocation, workingLocation):
    bp = settings.CHUNK_SIZE

    #Take the file name and split it at each delim.
    #Then check to make sure that we have all 6 identifiers:
    #Kingdom, Class, Genus, Species, Specimen, Chromosome
    fileName = re.sub('\.fasta', '', file)
    fileName = re.sub('\.fa', '', fileName)

    #Parse file name into system path
    taxonomic = fileName.split("_")

    if len(taxonomic) != 6:
        raise IOError("Error! File " + fileName + " in to_import is not validly named!")

    #Check to see if this specific file has already been split up and is stored in the system
    if HasFastaFile(taxonomic[4], taxonomic[5]):
        print "This sample is already stored in the system!"
        return False
    print "Entering this sample into the system..."

    filePath = os.path.join(storageLocation)
    pngFilePath = os.path.join(storageLocation).replace("fasta", "png")
    for sub in taxonomic:
        filePath = os.path.join(filePath, sub)
        pngFilePath = os.path.join(pngFilePath, sub)
    if not os.path.isdir(filePath):
        os.makedirs(filePath)
    if not os.path.isdir(pngFilePath):
        os.makedirs(pngFilePath)

    #Begin setting up the Specimen object for the database
    specimen, created = Specimen.objects.get_or_create(Name=taxonomic[4], Species=taxonomic[3], Genus=taxonomic[2],
                                                       Class=taxonomic[1], Kingdom=taxonomic[0])
    if created:
        specimen.Public = True
        specimen.GenomeLength = 0
        specimen.save()

    #Begin setting up the FastaFile object for the database
    fastaFile = FastaFiles()
    fastaFile.Specimen = specimen
    fastaFile.Chromosome = taxonomic[5]

    #Array of fasta chunks
    fastaChunks = []

    #Remove first line if needed and depending on OS
    if os.name == "posix":
        #FAST SED COMMAND ON LINUX
        bashCommand = "sed -i -e '1{/>/d;}' " + (workingLocation + file)
        os.system(bashCommand)
    else: #SLOW WINDOWS VERSION
        skip = False
        with open(workingLocation + file) as f:
            while True:
                character = f.read(1)
                if character:
                    if character == ">":
                        skip = True
                        break
                    if character == "\n":
                        break
                else:
                    break
        if skip:
            lines = open(workingLocation + file).readlines()
            open(workingLocation + file, 'w').writelines(lines[1:])

    #Split the file every $bp
    with open(workingLocation + file) as f:
        fCount = 1
        cCount = 0
        chunk = ""
        while True:
            character = f.read(1)
            if character:
                if character != "\n" and character != "\r" and character != ' ':
                    chunk += character
                    cCount += 1
                    if cCount == bp:
                        writePath = os.path.join(filePath, str(fCount) + ".fasta")
                        write = open(writePath, 'wb')
                        write.write(chunk.upper())
                        #Add this chunk to the list of chunks
                        newChunk = FastaChunkFiles()
                        newChunk.Start = fCount
                        fastaChunks.append(newChunk)
                        chunk = ""
                        fCount += cCount
                        cCount = 0
            else:
                break
        writePath = os.path.join(filePath, str(fCount) + ".fasta")
        write = open(writePath, 'wb')
        write.write(chunk.upper())
        newChunk = FastaChunkFiles()
        newChunk.Start = fCount
        fastaChunks.append(newChunk)
        fastaFile.Length = fCount + cCount - 1

    #Save fasta file to database then populate chunks with foreign keys and save
    fastaFile.save()
    specimen.GenomeLength += fastaFile.Length
    specimen.save()

    for fa in fastaChunks:
        fa.FastaFile = fastaFile
        fa.save()

    print "Done enterting " + taxonomic[4] + " " + taxonomic[5] + " into the system!"
    return True

#----------------------------------------------------------------------------------------
def run():
    #CD into the folder where this file is located as it should be the DNAStorage folder
    workingDir = settings.SKITTLE_TREE_LOC + "DNAStorage"
    os.chdir(workingDir)

    #Look to see if there are any files in to_import
    #If so, then process them.
    for file in os.listdir("./to_import/"):
        if file.endswith(".fasta") or file.endswith(".fa"):
            try:
                splitAndSort(file, workingDir + "/fasta", workingDir + "/to_import/")
                shutil.move("to_import/" + file, "history/" + file)
            except IOError as ex:
                print ex
                shutil.move("to_import/" + file, "rejected/" + file)

def Import(fileLoc, kingdom, classtype, genus, species, specimen, chromosome):
    pass
