from models import FastaFiles, FastaChunkFiles, ImageFiles
from SkittleTree import settings

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
            fastaFilePath = settings.SkittleTreeLoc + "fasta/" + fastaFile[0].FastaFile.Kingdom + "/" + fastaFile[0].FastaFile.Class + "/" + fastaFile[0].FastaFile.Genus + "/" + fastaFile[0].FastaFile.Species + "/" + fastaFile[0].FastaFile.Specimen + "/" + fastaFile[0].FastaFile.Chromosome + "/" + str(fastaFile[0].Start) + ".fasta"
            
        return fastaFilePath
    else: return None

#Return the number of characters logged for a specific chromosome    
def GetChromosomeLength(specimen, chromosome):
    chr = FastaFiles.objects.filter(Specimen = specimen, Chromosome = chromosome)[:1]
    
    if chr:
        return chr[0].Length
    else:
        return 0
    
#Searches to see if the given image file is stored in the system. If so, it returns the system path to the requested chunk
def GetPngFilePath(specimen, chromosome, graph, start, scale = None, charsPerLine = None):
    pngFile = ImageFiles.objects.filter(FastaFile__Specimen = specimen, FastaFile__Chromosome = chromosome, Start = start, Scale = scale, CharactersPerLine = charsPerLine)[:1]
    
    if pngFile:
        #Check if image file is stored in ram disk
        if pngFile[0].IsInRamDisk:
            pngFilePath = None
        else:
            thisStart = "_start=" + str(start)
            if scale:
                thisScale = "_scale=" + str(scale)
            else:
                thisScale = ""
            if charsPerLine:
                thisCharsPerLine = "_charactersperline=" + str(charsPerLine)
            else:
                thisCharsPerLine = ""
            pngFilePath = settings.SkittleTreeLoc + "png/" + pngFile[0].FastaFile.Kingdom + "/" + pngFile[0].FastaFile.Class + "/" + pngFile[0].FastaFile.Genus + "/" + pngFile[0].FastaFile.Species + "/" + pngFile[0].FastaFile.Specimen + "/" + pngFile[0].FastaFile.Chromosome + "/" + graph + thisStart + thisScale + thisCharsPerLine + ".png"
        print pngFilePath
    else: return None
    
#Take params and write a png to the disk and create a reference to it in the DB
def StorePng(specimen, chromosome, graph, start, scale = None, charsPerLine = None):
    print "NONE!"
    