import os
import re
import shutil

from django.conf import settings
from django.db import transaction, connection

from models import FastaFiles, FastaChunkFiles, Specimen, ImportProgress
import StorageRequestHandler


def saveDBObject(object):
    transaction.enter_transaction_management()
    transaction.commit()
    try:
        object.save()
    except:
        print "Retrying DB query!"
        connection.close()
        object.save()

def parseChromosome(fileName):
    known = ["chromosome", "ch", "chr", "chro", "chrom"]

    parts = re.split('-|_|\.|\ ', fileName)

    if len(parts) > 1:
        for part in parts:
            for sample in known:
                if sample in part.lower().strip():
                    return part.lower().strip()
        return parts[-1].lower().strip()
    else:
        parts = parts[0]
        for sample in known:
            if sample in parts.lower().strip():
                location = re.search(sample, parts)
                return parts[location.start():]
        return parts

#Take a fasta file, split it and sort it into the correct folders
def splitAndSort(file, storageLocation, workingLocation, attributes=None, progress=None):
    bp = settings.CHUNK_SIZE

    #Take the file name and split it at each delim.
    #Then check to make sure that we have all 6 identifiers:
    #Kingdom, Class, Genus, Species, Specimen, Chromosome
    fileName = re.sub('\.fasta', '', file)
    fileName = re.sub('\.fa', '', fileName)

    if progress:
        progress.Message = "Starting to parse taxonomic attributes..."
        progress.IsWorking = True
        progress.Success = False
        saveDBObject(progress)

    if not attributes:
        #Parse file name into system path
        taxonomic = fileName.split("_")
    else:
        taxonomic = []
        #Attributes(kingdom, class, genus, species, specimen, genomeName, source, dateSequenced, description, isPublic)
        taxonomic.append(attributes.get('kingdom', "Uncategorized") or "Uncategorized")
        taxonomic.append(attributes.get('class', "Uncategorized") or "Uncategorized")
        taxonomic.append(attributes.get('genus', "Uncategorized") or "Uncategorized")
        taxonomic.append(attributes.get('species', "Uncategorized") or "Uncategorized")
        taxonomic.append(attributes['specimen'])
        taxonomic.append(attributes.get('chromosome', parseChromosome(fileName)) or parseChromosome(fileName))

    if len(taxonomic) != 6 and not attributes:
        raise IOError("Error! File " + fileName + " in to_import is not validly named!")
    if not taxonomic[4] or not taxonomic[5]:
        message = "Error! Specimen and/or Chromosome name not valid!"
        print message
        progress.Message = message
        progress.IsWorking = False
        progress.Success = False
        saveDBObject(progress)
        return False

    print "Entering Specimen and attributes into the system..."
    if progress:
        progress.Message = "Entering Specimen and attributes into the system..."
    #Begin setting up the Specimen object for the database
    #specimen, created = Specimen.objects.get_or_create(Name=taxonomic[4], Species=taxonomic[3], Genus=taxonomic[2], Class=taxonomic[1], Kingdom=taxonomic[0])
    hasSpecimen = StorageRequestHandler.HasSpecimen(taxonomic[4])


    oldKingdom = ""
    oldClass = ""
    oldGenus = ""
    oldSpecies = ""
    pathChanged = False
    if not hasSpecimen:
        specimen = Specimen(Name=taxonomic[4], Species=taxonomic[3], Genus=taxonomic[2], Class=taxonomic[1], Kingdom=taxonomic[0])
    else:
        specimen = StorageRequestHandler.GetSpecimen(taxonomic[4])
        if attributes:
            oldKingdom = specimen.Kingdom
            oldClass = specimen.Class
            oldGenus = specimen.Genus
            oldSpecies = specimen.Species

            specimen.Kingdom = attributes.get('kingdom', specimen.Kingdom) or specimen.Kingdom
            specimen.Class = attributes.get('class', specimen.Class) or specimen.Class
            specimen.Genus = attributes.get('genus', specimen.Genus) or specimen.Genus
            specimen.Species = attributes.get('species', specimen.Species) or specimen.Species
            taxonomic[0] = specimen.Kingdom
            taxonomic[1] = specimen.Class
            taxonomic[2] = specimen.Genus
            taxonomic[3] = specimen.Species

            if (oldKingdom.lower() != specimen.Kingdom.lower()) or (oldClass.lower() != specimen.Class.lower()) or (oldGenus.lower() != specimen.Genus.lower()) or (oldSpecies.lower() != specimen.Species.lower()):
                pathChanged = True

    if attributes:
        specimen.ExtendedName = attributes.get('genomeName', specimen.ExtendedName) or specimen.ExtendedName
        specimen.Source = attributes.get('source', specimen.Source) or specimen.Source
        specimen.Description = attributes.get('description', specimen.Description) or specimen.Description
        specimen.DatePublished = attributes.get('dateSequenced', specimen.DatePublished) or specimen.DatePublished

    saveDBObject(specimen)

    #Check to see if this specific file has already been split up and is stored in the system
    if StorageRequestHandler.HasFastaFile(taxonomic[4], taxonomic[5]):
        message = "This sample is already stored in the system!"
        print message
        if progress:
            progress.Message = message
            progress.IsWorking = False
            progress.Success = False
            saveDBObject(progress)
        return False

    print "Setting up server storage..."
    if progress:
        progress.Success = False
        progress.Message = "Settup up server storage..."
        saveDBObject(progress)

    filePath = os.path.join(storageLocation)
    pngFilePath = os.path.join(storageLocation).replace("fasta", "png")
    for sub in taxonomic:
        filePath = os.path.join(filePath, sub)
        pngFilePath = os.path.join(pngFilePath, sub)
    if not os.path.isdir(filePath):
        os.makedirs(filePath)
    if not os.path.isdir(pngFilePath):
        os.makedirs(pngFilePath)

    if pathChanged:
        oldFilePath = os.path.join(storageLocation)
        oldPNGFilePath = os.path.join(storageLocation).replace("fasta", "png")

        oldFilePath = os.path.join(oldFilePath, oldKingdom)
        oldFilePath = os.path.join(oldFilePath, oldClass)
        oldFilePath = os.path.join(oldFilePath, oldGenus)
        oldFilePath = os.path.join(oldFilePath, oldSpecies)
        oldFilePath = os.path.join(oldFilePath, specimen.Name)

        oldPNGFilePath = os.path.join(oldPNGFilePath, oldKingdom)
        oldPNGFilePath = os.path.join(oldPNGFilePath, oldClass)
        oldPNGFilePath = os.path.join(oldPNGFilePath, oldGenus)
        oldPNGFilePath = os.path.join(oldPNGFilePath, oldSpecies)
        oldPNGFilePath = os.path.join(oldPNGFilePath, specimen.Name)

        #Move all chunks to new destination
        newFilePath = os.path.dirname(filePath)
        newPNGFilePath = os.path.dirname(pngFilePath)
        shutil.move(oldFilePath, newFilePath)
        shutil.move(oldPNGFilePath, newPNGFilePath)

    print "Entering this sample into the system..."
    if progress:
        #Mark that we are now starting processing
        progress.IsWorking = True
        progress.Message = "Entering this sample into the system..."
        saveDBObject(progress)

    #Begin setting up the FastaFile object for the database
    fastaFile = FastaFiles()
    fastaFile.Specimen = specimen
    fastaFile.Chromosome = taxonomic[5]

    #Mark if this chromosome is public or not
    if not attributes or attributes['isPublic']:
        fastaFile.Public = True
    else:
        fastaFile.Public = False

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
    print "Done entering sample!"
    if progress:
        print "Saving to user's profile."
        progress.Message = "Done entering sample! Saving to user's profile."
        saveDBObject(progress)
        saveDBObject(fastaFile)
        fastaFile.User.add(progress.User.all()[0])
    saveDBObject(fastaFile)

    print "Adding sample length to specimen."
    if progress:
        progress.Message = "Adding sample length to specimen."
        saveDBObject(progress)
    specimen.GenomeLength += fastaFile.Length
    saveDBObject(specimen)

    for fa in fastaChunks:
        fa.FastaFile = fastaFile
        saveDBObject(fa)

    message = "Done entering " + taxonomic[4] + " " + taxonomic[5] + " into the system!"
    print message
    if progress:
        progress.Message = message
        progress.IsWorking = False
        progress.Success = True
        progress.FastaFile = fastaFile
        saveDBObject(progress)
        # TODO Notify the user that their file is completely imported
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


def ImportFasta(fileName, attributes, user):
    #Attributes(kingdom, class, genus, species, specimen, genomeName, source, dateSequenced, description)
    
    workingDir = settings.SKITTLE_TREE_LOC + "DNAStorage"
    os.chdir(workingDir)

    if fileName.endswith(".fasta") or fileName.endswith(".fa"):
        if not user:
            return False

        progress, created = ImportProgress.objects.get_or_create(Specimen=attributes['specimen'], FileName=fileName)
        if not created:
            if progress.IsWorking:
                progress.Message = "Ignoring duplicate file upload..."
                saveDBObject(progress)
                return False
        progress.User.add(user)
        saveDBObject(progress)

        from multiprocessing import Process
        importer = Process(target=importFasta, args=(workingDir, fileName, attributes, progress))
        importer.start()

        return progress.id
    else:
        print "This is not a fasta file!"
        shutil.move("to_import/" + fileName, "rejected/" + fileName)
        return "This is not a fasta file... Import failed!"


def importFasta(workingDir, fileName, attributes, progress):
    try:
        splitAndSort(fileName, workingDir + "/fasta", workingDir + "/to_import/", attributes=attributes, progress=progress)
        shutil.move("to_import/" + fileName, "history/" + fileName)
    except IOError as ex:
        print ex
        shutil.move("to_import/" + fileName, "rejected/" + fileName)
        return ex.message
