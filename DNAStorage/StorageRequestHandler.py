from models import FastaFiles, FastaChunkFiles, ImageFiles

def HasFastaFile(specimen, chromosome):
    has = FastaFiles.objects.filter(Specimen = specimen, Chromosome = chromosome)
    return has