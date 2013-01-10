import sys, os, os.path, re

#Take a fasta file, split it and sort it into the correct folders
def splitAndSort(file, storageLocation):
    bp = 65536
    
    #Take the file name and split it at each delim.
    #Then check to make sure that we have all 6 identifiers:
    #Kingdom, Class, Genus, Species, Specimen, Chromosome
    #TODO: Convert this regex to be one line instead of two
    fileName = re.sub('\.fa[sta]', '', file)
    
    #Parse file name into system path
    taxonomic = fileName.split("_")
    
    if len(taxonomic) != 6:
        raise IOError("Error! File " + fileName + " in to_import is not validly named!")
    
    filePath = os.path.join(storageLocation)
    for sub in taxonomic:
        filePath = os.path.join(filePath, sub)   
    if not os.path.isdir(filePath):
        os.makedirs(filePath)
        
    #Split the file every $bp
    

#----------------------------------------------------------------------------------------
#CD into the folder where this file is located as it should be the DNAStorage folder
workingDir = os.path.realpath(__file__).replace("\\", "/")
workingDir = re.sub('/ProcessFasta\.py', '', workingDir)
os.chdir(workingDir)

#Look to see if there are any files in to_import
#If so, then process them.
for file in os.listdir("./to_import/"):
    if file.endswith(".fasta") or file.endswith(".fa"):
        try:
            splitAndSort(file, workingDir + "/fasta")
            os.rename("to_import/" + file, "history/" + file)
        except IOError as ex:
            print ex
            os.rename("to_import/" + file, "rejected/" + file)
