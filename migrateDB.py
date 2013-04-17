import MySQLdb
from django.conf import settings

def setupDB():
    db = None

    if settings.PRODUCTION:
        db = MySQLdb.connect(host="localhost", user="skittle", passwd="sk!77l3PandaDatabase%", db="DNASkittle")
        dbName = "dnaskittle"
    else:
        db = MySQLdb.connect(host="localhost", user="skittle", passwd="sk!77l3PandaDatabase%", db="SkittleTree")
        dbName = "skittletree"
    
    cur = db.cursor()
    return cur

#This is where we will be placing the code for migrations.
#What we are doing here is injecting base SQL commands.
#Each migration will have a unique set of SQL commands which will only upgrade from the directly prior version.

#####_____MIGRATION 1_____#####
def RunMigration1():
    cur = setupDB()
    
    part1 = "CREATE TABLE `Annotations_gff` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `Specimen_id` integer NOT NULL, `GFFVersion` smallint NOT NULL, `SourceVersion` varchar(255), `Date` date, `Type` varchar(255), `DNA` longtext, `RNA` longtext, `Protein` longtext, `SequenceRegion` varchar(255), `FileName` varchar(255) NOT NULL)"
    part2 = "CREATE TABLE `Annotations_annotationjsonchunk` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `GFF_id` integer NOT NULL, `Chromosome` varchar(25) NOT NULL, `Start` integer NOT NULL, `IsInRamDisk` bool NOT NULL)"

    part3 = "ALTER TABLE `Annotations_annotationjsonchunk` ADD CONSTRAINT `GFF_id_refs_id_45f1aae6` FOREIGN KEY (`GFF_id`) REFERENCES `Annotations_gff` (`id`)"
    part4 = "ALTER TABLE `Annotations_gff` ADD CONSTRAINT `Specimen_id_refs_id_1c12cb2c` FOREIGN KEY (`Specimen_id`) REFERENCES `DNAStorage_specimen` (`id`)"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    
#####_____MIGRATION 2_____#####
def RunMigration2():
    cur = setupDB()
    
    part1 = "ALTER TABLE `Annotations_gff` MODIFY COLUMN `GFFVersion` SMALLINT(6) DEFAULT NULL;"
    
    cur.execute(part1)
    
#####_____MIGRATION 3_____#####
def RunMigration3():
    cur = setupDB()
    
    part1 = "ALTER TABLE `Annotations_annotationjsonchunk` MODIFY COLUMN `Start` BIGINT(20) NOT NULL;"
    
    cur.execute(part1)
    
#####_____MIGRATION 4_____#####
def RunMigration4():
    cur = setupDB()
    
    part1 = "CREATE TABLE `Annotations_snpindexinfo` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `SnpName` varchar(255) NOT NULL, `Chromosome` varchar(25) NOT NULL, `Start` bigint NOT NULL, `CompactIndex` bigint NOT NULL)"
    
    cur.execute(part1)
    
#####_____MIGRATION 5_____#####
def RunMigration5():
    cur = setupDB()
    
    part1 = "ALTER TABLE `Annotations_snpindexinfo` CHANGE COLUMN `Start` `Temp` BIGINT(20) NOT NULL"
    part2 = "ALTER TABLE `Annotations_snpindexinfo` CHANGE COLUMN `CompactIndex` `Start` BIGINT(20) NOT NULL"
    part3 = "ALTER TABLE `Annotations_snpindexinfo` CHANGE COLUMN `Temp` `CompactIndex` BIGINT(20) NOT NULL"
    
    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    
#####_____MIGRATION 6_____#####
def RunMigration6():
    cur = setupDB()
    
    part1 = "ALTER TABLE `DNAStorage_specimen` ADD COLUMN `Public` TINYINT(1) UNSIGNED NOT NULL AFTER `Thumbnail`"
    
    cur.execute(part1)
    
    #Set all imported chromosomes to default public for this first and only run of migration
    from DNAStorage.models import Specimen
    specimens = Specimen.objects.all()
    for specimen in specimens:
        specimen.Public = True
        specimen.save()
        
#####_____MIGRATION 7_____#####
def RunMigration7():
    cur = setupDB()
    
    part1 = "CREATE TABLE `SkittleCore_processqueue` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `Specimen` varchar(200) NOT NULL, `Chromosome` varchar(200) NOT NULL, `Graph` varchar(1) NOT NULL, `Start` integer NOT NULL, `Scale` integer, `CharsPerLine` integer)"
    
    cur.execute(part1)