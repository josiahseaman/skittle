from models import FastaFiles, FastaChunkFiles, ImageFiles
from SkittleTree import settings
import shutil, os, os.path, re

#Returns if the system contains the requested fasta file. This does NOT return full data associated with it for speed purposes.
def HasFastaFile(specimen, chromosome):
    has = FastaFiles.objects.filter(Specimen = specimen, Chromosome = chromosome)[:1]
    return has
    
#Searches to see if the given fasta file is stored in the system. If so, it returns the system path to the requested chunk
def GetFastaFilePath(specimen, chromosome, start):
    fastaFile = FastaChunkFiles.objects.filter(FastaFile__Specimen = specimen, FastaFile__Chromosome = chromosome, Start = start)[:1]
    if fastaFile:
        #Check if fasta file is stored in ram disk
        if fastaFile[0].IsInRamDisk:
            fastaFilePath = None
        else:
            fastaFilePath = settings.SkittleTreeLoc + "DNAStorage/fasta/" + fastaFile[0].FastaFile.Kingdom + "/" + fastaFile[0].FastaFile.Class + "/" + fastaFile[0].FastaFile.Genus + "/" + fastaFile[0].FastaFile.Species + "/" + fastaFile[0].FastaFile.Specimen + "/" + fastaFile[0].FastaFile.Chromosome + "/" + str(fastaFile[0].Start) + ".fasta"
            
        return fastaFilePath
    else: return None

#Return the number of characters logged for a specific chromosome    
def GetChromosomeLength(specimen, chromosome):
    chr = FastaFiles.objects.filter(Specimen = specimen, Chromosome = chromosome)[:1]
    
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
    
    pngFile = ImageFiles.objects.filter(FastaFile__Specimen = specimen, FastaFile__Chromosome = chromosome, Start = start, Scale = scale, CharactersPerLine = charsPerLine)[:1]
    
    if pngFile:
        #Check if image file is stored in ram disk
        if pngFile[0].IsInRamDisk:
            pngFilePath = None
        else:
            pngFilePath = settings.SkittleTreeLoc + "DNAStorage/png/" + pngFile[0].FastaFile.Kingdom + "/" + pngFile[0].FastaFile.Class + "/" + pngFile[0].FastaFile.Genus + "/" + pngFile[0].FastaFile.Species + "/" + pngFile[0].FastaFile.Specimen + "/" + pngFile[0].FastaFile.Chromosome + "/" + generatePngName(graph, start, scale, charsPerLine)
        return pngFilePath
    else: 
        return None
    
#Take params and write a png to the disk and create a reference to it in the DB
def StorePng(request, fileObject):
    #assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width
    
    #Get the Related FastaFile
    fastaFile = FastaFiles.objects.filter(Specimen = specimen, Chromosome = chromosome)[:1]
    
    if fastaFile:
        fastaFile = fastaFile[0]
    else:
        raise "No associated FastaFile for this PNG!"
        return None
    
    #Move temp file from temp storage into cache storage
    pngFilePath = settings.SkittleTreeLoc + "DNAStorage/png/" + fastaFile.Kingdom + "/" + fastaFile.Class + "/" + fastaFile.Genus + "/" + fastaFile.Species + "/" + fastaFile.Specimen + "/" + fastaFile.Chromosome + "/" + generatePngName(graph, start, scale, charsPerLine)
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
def DeleteCache(graph):
    #Delete database entries first
    oldFiles = ImageFiles.objects.filter(GraphType = graph).delete()
    
    #Now remove PNG files
    #CD into the folder where this file is located as it should be the DNAStorage folder
    workingDir = settings.SkittleTreeLoc + "DNAStorage/png/"
    
    graphString = graph + "_"
    
    for root, dirs, files in os.walk(workingDir):
        for f in files:
            fullpath = os.path.join(root, f)
            if graphString in f:
                os.remove(fullpath)
