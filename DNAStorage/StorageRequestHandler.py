import shutil
import os

from django.conf import settings

from models import *
from django.db.models import Q
from Utilities.SkittleUtils import GetRoundedIndex
import ProcessFasta


def HasSpecimen(specimen):
    has = Specimen.objects.filter(Name__iexact=specimen)[:1]
    return has

#Returns if the system contains the requested fasta file. This does NOT return full data associated with it for speed purposes.
def HasFastaFile(specimen, chromosome):
    has = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]
    return has

#Searches to see if the given fasta file is stored in the system. If so, it returns the system path to the requested chunk
def GetFastaFilePath(specimen, chromosome, start):
    fastaFile = FastaChunkFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome,
                                               Start=start)[:1]
    if fastaFile:
        #Check if fasta file is stored in ram disk
        if fastaFile[0].IsInRamDisk:
            fastaFilePath = None
        else:
            fastaFilePath = settings.SKITTLE_TREE_LOC + "DNAStorage/fasta/" + fastaFile[0].FastaFile.Specimen.Kingdom + "/" + fastaFile[0].FastaFile.Specimen.Class + "/" + fastaFile[0].FastaFile.Specimen.Genus + "/" + fastaFile[0].FastaFile.Specimen.Species + "/" + fastaFile[0].FastaFile.Specimen.Name + "/" + fastaFile[0].FastaFile.Chromosome + "/" + str(fastaFile[0].Start) + ".fasta"

        return fastaFilePath
    else:
        return None

#Return the number of characters logged for a specific chromosome    
def GetChromosomeLength(specimen, chromosome):
    chr = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]

    if chr:
        return chr[0].Length
    else:
        return 0

#Take params of request and generate what the filename of a png should be (no path included)
def generatePngName(graph, start, scale, charsPerLine):
    thisStart = "_start=" + str(start)
    if scale:
        thisScale = "_scale=" + str(scale)
    else:
        thisScale = ""
    if charsPerLine:
        thisCharsPerLine = "_charactersperline=" + str(charsPerLine)
    else:
        thisCharsPerLine = ""

    return graph + thisStart + thisScale + thisCharsPerLine + ".png"

#Searches to see if the given image file is stored in the system. If so, it returns the system path to the requested chunk
def GetPngFilePath(request):
    #assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

    pngFile = ImageFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome,
                                        Start=start, Scale=scale, CharactersPerLine=charsPerLine)[:1]

    if pngFile:
        #Check if image file is stored in ram disk
        if pngFile[0].IsInRamDisk:
            pngFilePath = None
        else:
            pngFilePath = settings.SKITTLE_TREE_LOC + "DNAStorage/png/" + pngFile[0].FastaFile.Specimen.Kingdom + "/" + pngFile[0].FastaFile.Specimen.Class + "/" + pngFile[0].FastaFile.Specimen.Genus + "/" + pngFile[0].FastaFile.Specimen.Species + "/" + pngFile[0].FastaFile.Specimen.Name + "/" + pngFile[0].FastaFile.Chromosome + "/" + generatePngName(graph, start, scale, charsPerLine)
        return pngFilePath
    else:
        return None

#Take params and write a png to the disk and create a reference to it in the DB
def StorePng(request, fileObject):
    #assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

    #Get the Related FastaFile
    fastaFile = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]

    if fastaFile:
        fastaFile = fastaFile[0]
    else:
        raise Exception("No associated FastaFile for this PNG!")
        return None

    #Move temp file from temp storage into cache storage
    pngFilePath = settings.SKITTLE_TREE_LOC + "DNAStorage/png/" + fastaFile.Specimen.Kingdom + "/" + fastaFile.Specimen.Class + "/" + fastaFile.Specimen.Genus + "/" + fastaFile.Specimen.Species + "/" + fastaFile.Specimen.Name + "/" + fastaFile.Chromosome + "/" + generatePngName(
        graph, start, scale, charsPerLine)
    shutil.copyfile(fileObject.name, pngFilePath)

    #Log this file in the database
    imageFile = ImageFiles()
    imageFile.FastaFile = fastaFile
    imageFile.GraphType = graph
    imageFile.Start = start
    imageFile.Scale = scale
    imageFile.CharactersPerLine = charsPerLine

    imageFile.save()

    return imageFile

#Delete the database entries and PNG files associated with the given graph
def DeleteCache(graph, specimen, chromosome, start):
    #Delete database entries first
    if start and chromosome and specimen:
        start = GetRoundedIndex(start)
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome, Start=start).delete()
    elif chromosome and specimen:
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome).delete()
    elif specimen:
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen).delete()
    else:
        ImageFiles.objects.filter(GraphType=graph).delete()

    #Now remove PNG files
    #CD into the folder where this file is located as it should be the DNAStorage folder
    workingDir = settings.SKITTLE_TREE_LOC + "DNAStorage/png/"
    if specimen:
        s = GetSpecimen(specimen)
        workingDir += s.Kingdom + "/" + s.Class + "/" + s.Genus + "/" + s.Species + "/" + str(specimen).strip() + "/"
        if chromosome:
            workingDir += str(chromosome).strip() + "/"

    graphString = graph + "_"
    if specimen and chromosome and start:
        graphString += "start=" + str(start).strip() + "_"

    for root, dirs, files in os.walk(workingDir):
        for f in files:
            fullpath = os.path.join(root, f)
            if graphString in f:
                os.remove(fullpath)

#Get a python object containing a unique tree that travels to the specimens and contains the chromosome files of each specimen
def GetTreeList(user=None):
    tree = {}
    #tree = {"Kingdom": {"Class": {"Genus": {"Species": {"Specimen": {"ExtendedName", "Source", "Description", "DatePublished", "Thumbnail", {"ChromosomeListing",}}}}}}}

    specimens = Specimen.objects.all()
    for entry in specimens:
        #Go through all chromosomes related to this specimen and generate list
        chromosomeList = []
        chromosomes = FastaFiles.objects.filter(Specimen=entry)
        for chromosome in chromosomes:
            if chromosome.Public:
                chromosomeList.append(chromosome.Chromosome)
            elif user:
                if user in chromosome.User.all():
                    chromosomeList.append(chromosome.Chromosome)
            else:
                chromosomeList.append(chromosome.Chromosome)

        if len(chromosomeList) > 0:
            #Gather all specimen details (including chromosomes) into list
            details = {"ExtendedName": entry.ExtendedName, "GenomeLength": entry.GenomeLength, "Source": entry.Source, "Description": entry.Description,
                   "DatePublished": entry.DatePublished, "Thumbnail": entry.Thumbnail, "Chromosomes": chromosomeList}

            if not entry.Kingdom in tree:
                tree[entry.Kingdom] = {}
            if not entry.Class in tree[entry.Kingdom]:
                tree[entry.Kingdom][entry.Class] = {}
            if not entry.Genus in tree[entry.Kingdom][entry.Class]:
                tree[entry.Kingdom][entry.Class][entry.Genus] = {}
            if not entry.Species in tree[entry.Kingdom][entry.Class][entry.Genus]:
                tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species] = {}
            if not entry.Name in tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species]:
                tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species][entry.Name] = {}
            tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species][entry.Name] = details

    return tree

#Get list of chromosomes related to a specimen
def GetRelatedChromosomes(specimen, user):
    if user.is_authenticated() : fastaFiles =  FastaFiles.objects.filter(Q(Public=True) | Q(User=user))
    else: fastaFiles =  FastaFiles.objects.filter(Public=True)
    if type(specimen) is unicode: 
        fastaFiles = fastaFiles.filter(Specimen__Name__iexact=specimen)
    else:
        fastaFiles = fastaFiles.filter(Specimen=specimen)

    fastaFiles = fastaFiles.order_by('Chromosome')

    chromosomes = []

    for fasta in fastaFiles:
        chromosomes += [fasta.Chromosome]

    return chromosomes

#Get the FastaFile related to the given specimen's chromosome
def GetRelatedFastaFile(specimen, chromosome):
    fastaFile = FastaFiles.objects.filter(Specimen=specimen, Chromosome__iexact=chromosome)[:1]

    if fastaFile:
        return fastaFile[0]
    else:
        return None

#Get the fasta chunk file at the given start position for the specified chromosome
def GetFastaChunkFile(specimen, chromosome, start):
    fastaChunkFile = FastaChunkFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen,
                                                    FastaFile__Chromosome__iexact=chromosome, Start=start)[:1]

    if fastaChunkFile:
        return fastaChunkFile[0]
    else:
        return None

#Get the specimen in the database with the given name
def GetSpecimen(specimen):
    specimen = Specimen.objects.filter(Name__iexact=specimen)[:1]

    if specimen:
        return specimen[0]
    else:
        return None

def GetImportProgress(specimen, fileName):
    progress = ImportProgress.objects.filter(Specimen=specimen, FileName=fileName)[:1]

    if progress:
        return progress[0]
    else:
        return None

def GetUserImports(user):
    uploads = ImportProgress.objects.filter(User=user)
    return uploads

def GetUserFastas(user):
    fastas = FastaFiles.objects.filter(User=user)
    return fastas

def HandleUploadedFile(f, attributes, user):
    try:
        with open(settings.SKITTLE_TREE_LOC + "DNAStorage/to_import/" + f.name, 'wb+') as destination:
            for chunk in f.chunks():
                destination.write(chunk)
    except:
        return "Error uploading file!"
    return ProcessFasta.ImportFasta(f.name, attributes, user)

def IsUserForbidden(specimen, chromosome, user):
    chromosome = GetRelatedFastaFile(GetSpecimen(specimen), chromosome)

    if chromosome and (chromosome.Public or chromosome.User == user):
        return False
    return True
