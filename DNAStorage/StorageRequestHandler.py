import shutil
import os

from django.conf import settings
from math import floor

from models import *
from django.db.models import Q
from Utilities.SkittleUtils import GetRoundedIndex
import ProcessFasta


def HasSpecimen(specimen):
    has = Specimen.objects.filter(Name__iexact=specimen)[:1]
    return has


def HasFastaFile(specimen, chromosome):
    """Returns if the system contains the requested fasta file. This does NOT return full data associated with it for speed purposes."""
    has = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]
    return has


def GetFastaFilePath(specimen, chromosome, start):
    """Searches to see if the given fasta file is stored in the system. If so, it returns the system path to the requested chunk"""
    roundedStart = int(floor(start / settings.CHUNK_SIZE) * settings.CHUNK_SIZE) + 1
    fastaFile = FastaChunkFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome,
                                               Start=roundedStart)[:1]
    if fastaFile:
        # Check if fasta file is stored in ram disk
        if fastaFile[0].IsInRamDisk:
            fastaFilePath = None
        else:
            fastaFilePath = os.path.join(settings.BASE_DIR, "DNAStorage", "fasta",
                                         fastaFile[0].FastaFile.Specimen.Kingdom,
                                         fastaFile[0].FastaFile.Specimen.Class,
                                         fastaFile[0].FastaFile.Specimen.Genus,
                                         fastaFile[0].FastaFile.Specimen.Species,
                                         fastaFile[0].FastaFile.Specimen.Name,
                                         fastaFile[0].FastaFile.Chromosome,
                                         str(fastaFile[0].Start) + ".fasta")
        return fastaFilePath
    else:
        return None


def GetChromosomeLength(specimen, chromosome):
    """Return the number of characters logged for a specific chromosome"""
    chr = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]

    if chr:
        return chr[0].Length
    else:
        return 0


def generatePngName(graph, start, scale, charsPerLine):
    """Take params of request and generate what the filename of a png should be (no path included)"""
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


def GetPngFilePath(request):
    """Searches to see if the given image file is stored in the system. If so, it returns the system path to the requested chunk"""
    # assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

    pngFile = ImageFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome,
                                        Start=start, Scale=scale, CharactersPerLine=charsPerLine)[:1]

    if pngFile:
        # Check if image file is stored in ram disk
        if pngFile[0].IsInRamDisk:
            pngFilePath = None
        else:
            png_name = generatePngName(graph, start, scale, charsPerLine)
            pngFilePath = os.path.join(settings.BASE_DIR,
                                       "DNAStorage","png",
                                       pngFile[0].FastaFile.Specimen.Kingdom,
                                       pngFile[0].FastaFile.Specimen.Class,
                                       pngFile[0].FastaFile.Specimen.Genus,
                                       pngFile[0].FastaFile.Specimen.Species,
                                       pngFile[0].FastaFile.Specimen.Name,
                                       pngFile[0].FastaFile.Chromosome,
                                       png_name)
        return pngFilePath
    else:
        return None


def StorePng(request, fileObject):
    """Take params and write a png to the disk and create a reference to it in the DB"""
    # assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

    # Get the Related FastaFile
    fastaFile = FastaFiles.objects.filter(Specimen__Name__iexact=specimen, Chromosome__iexact=chromosome)[:1]

    if fastaFile:
        fastaFile = fastaFile[0]
    else:
        raise Exception("No associated FastaFile for this PNG!")
        return None

    # Move temp file from temp storage into cache storage
    png_name = generatePngName(graph, start, scale, charsPerLine)
    pngFilePath = os.path.join(settings.BASE_DIR,
                               "DNAStorage","png",
                               fastaFile.Specimen.Kingdom,
                               fastaFile.Specimen.Class,
                               fastaFile.Specimen.Genus,
                               fastaFile.Specimen.Species,
                               fastaFile.Specimen.Name,
                               fastaFile.Chromosome,
                               png_name)
    shutil.copyfile(fileObject.name, pngFilePath)

    # Log this file in the database
    imageFile = ImageFiles()
    imageFile.FastaFile = fastaFile
    imageFile.GraphType = graph
    imageFile.Start = start
    imageFile.Scale = scale
    imageFile.CharactersPerLine = charsPerLine

    imageFile.save()

    return imageFile


def DeleteCache(graph, specimen, chromosome, start):
    """Delete the database entries and PNG files associated with the given graph"""
    # Delete database entries first
    if start and chromosome and specimen:
        start = GetRoundedIndex(start)
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome, Start=start).delete()
    elif chromosome and specimen:
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen, FastaFile__Chromosome__iexact=chromosome).delete()
    elif specimen:
        ImageFiles.objects.filter(GraphType=graph, FastaFile__Specimen__Name__iexact=specimen).delete()
    else:
        ImageFiles.objects.filter(GraphType=graph).delete()

    # Now remove PNG files
    # CD into the folder where this file is located as it should be the DNAStorage folder
    workingDir = os.path.join(settings.BASE_DIR, "DNAStorage","png")
    if specimen:
        s = GetSpecimen(specimen)
        workingDir = os.path.join(workingDir, s.Kingdom, s.Class, s.Genus, s.Species, str(specimen).strip())
        if chromosome:
            workingDir = os.path.join(workingDir, str(chromosome).strip())

    graphString = graph + "_"
    if specimen and chromosome and start:
        graphString += "start=" + str(start).strip() + "_"

    for root, dirs, files in os.walk(workingDir):
        for f in files:
            fullpath = os.path.join(root, f)
            if graphString in f:
                os.remove(fullpath)


def GetTreeList(user=None):
    """Get a python object containing a unique tree that travels to the specimens and contains the chromosome files of each specimen"""
    tree = {}
    # tree = {"Kingdom": {"Class": {"Genus": {"Species": {"Specimen": {"ExtendedName", "Source", "Description", "DatePublished", "Thumbnail", {"ChromosomeListing",}}}}}}}

    specimens = Specimen.objects.all()
    for entry in specimens:
        # Go through all chromosomes related to this specimen and generate list
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
            # Gather all specimen details (including chromosomes) into list
            details = {"ExtendedName": entry.ExtendedName, "GenomeLength": entry.GenomeLength, "Source": entry.Source, "Description": entry.Description,
                   "DatePublished": entry.DatePublished, "Thumbnail": entry.Thumbnail, "Chromosomes": chromosomeList}

            if entry.Kingdom not in tree:
                tree[entry.Kingdom] = {}
            if entry.Class not in tree[entry.Kingdom]:
                tree[entry.Kingdom][entry.Class] = {}
            if entry.Genus not in tree[entry.Kingdom][entry.Class]:
                tree[entry.Kingdom][entry.Class][entry.Genus] = {}
            if entry.Species not in tree[entry.Kingdom][entry.Class][entry.Genus]:
                tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species] = {}
            if entry.Name not in tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species]:
                tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species][entry.Name] = {}
            tree[entry.Kingdom][entry.Class][entry.Genus][entry.Species][entry.Name] = details

    return tree


def GetRelatedChromosomes(specimen, user=None):
    """Get list of chromosomes related to a specimen"""
    if user is not None and user.is_authenticated():
        fastaFiles = FastaFiles.objects.filter(Q(Public=True) | Q(User=user))
    else: 
        fastaFiles = FastaFiles.objects.filter(Public=True)
    if isinstance(specimen, basestring):
        fastaFiles = fastaFiles.filter(Specimen__Name__iexact=specimen)
    else:
        fastaFiles = fastaFiles.filter(Specimen=specimen)

    fastaFiles = fastaFiles.order_by('Chromosome')

    chromosomes = []

    for fasta in fastaFiles:
        chromosomes += [fasta.Chromosome]

    return chromosomes


def GetRelatedFastaFile(specimen, chromosome):
    """Get the FastaFile related to the given specimen's chromosome"""
    fastaFile = FastaFiles.objects.filter(Specimen=specimen, Chromosome__iexact=chromosome)[:1]

    if fastaFile:
        return fastaFile[0]
    else:
        return None


def GetFastaChunkFile(specimen, chromosome, start):
    """Get the fasta chunk file at the given start position for the specified chromosome"""
    fastaChunkFile = FastaChunkFiles.objects.filter(FastaFile__Specimen__Name__iexact=specimen,
                                                    FastaFile__Chromosome__iexact=chromosome, Start=start)[:1]

    if fastaChunkFile:
        return fastaChunkFile[0]
    else:
        return None


def GetSpecimen(specimen):
    """Get the specimen in the database with the given name"""
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
        with open(os.path.join(settings.BASE_DIR, "DNAStorage", "to_import", f.name), 'wb+') as destination:
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
