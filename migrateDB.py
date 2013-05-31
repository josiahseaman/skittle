import MySQLdb
from django.conf import settings


def setupDB():
    db = None

    if settings.PRODUCTION:
        db = MySQLdb.connect(host="127.0.0.1", user="skittle", passwd="sk!77l3PandaDatabase%", db="DNASkittle")
        dbName = "dnaskittle"
    else:
        db = MySQLdb.connect(host="127.0.0.1", user="skittle", passwd="sk!77l3PandaDatabase%", db="SkittleTree")
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

#####_____MIGRATION 8_____#####
#Migration to remove session and visible from all graph states
#     WARNING!!!!!!!      YOU MUST DO THIS MIGRATION MANUALLY FOR NOW!!!
def RunMigration8():
    cur = setupDB()

    part1 = "ALTER TABLE `Graphs_annotationdisplaystate` DROP FOREIGN KEY `session_id_refs_id_65c151b6`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part2 = "ALTER TABLE `Graphs_highlighterstate` DROP FOREIGN KEY `session_id_refs_id_14ba4aba`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part3 = "ALTER TABLE `Graphs_nucleotidebiasstate` DROP FOREIGN KEY `session_id_refs_id_6c4b128d`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part4 = "ALTER TABLE `Graphs_nucleotidedisplaystate` DROP FOREIGN KEY `session_id_refs_id_452e3e19`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part5 = "ALTER TABLE `Graphs_oligomerusagestate` DROP FOREIGN KEY `session_id_refs_id_77954940`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part6 = "ALTER TABLE `Graphs_repeatmapstate` DROP FOREIGN KEY `session_id_refs_id_3dc95670`, DROP COLUMN `session_id`, DROP COLUMN `visible`"
    part7 = "ALTER TABLE `Graphs_threemerdetectorstate` DROP FOREIGN KEY `session_id_refs_id_3e50a710`, DROP COLUMN `session_id`, DROP COLUMN `visible`"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)
    cur.execute(part6)
    cur.execute(part7)

#####_____MIGRATION 9_____#####
def RunMigration9():
    cur = setupDB()

    part1 = "DROP TABLE IF EXISTS `Graphs_annotationdisplaystate`"
    partX = "DROP TABLE IF EXISTS `Graphs_sequenceentry`"
    part2 = "DROP TABLE IF EXISTS `Graphs_highlighterstate`"
    part3 = "DROP TABLE IF EXISTS `Graphs_nucleotidebiasstate`"
    part4 = "DROP TABLE IF EXISTS `Graphs_nucleotidedisplaystate`"
    part5 = "DROP TABLE IF EXISTS `Graphs_oligomerusagestate`"
    part6 = "DROP TABLE IF EXISTS `Graphs_repeatmapstate`"
    part7 = "DROP TABLE IF EXISTS `Graphs_threemerdetectorstate`"
    part8 = "DROP TABLE IF EXISTS `Graphs_similarityheatmapstate`"
    part9 = "DROP TABLE IF EXISTS `Graphs_oligomerusagestate`"

    cur.execute(part1)
    commitTrans()
    cur.execute(partX)
    commitTrans()
    cur.execute(part3)
    commitTrans()
    cur.execute(part4)
    commitTrans()
    cur.execute(part5)
    commitTrans()
    cur.execute(part6)
    commitTrans()
    cur.execute(part7)
    commitTrans()
    cur.execute(part8)
    commitTrans()
    cur.execute(part9)
    commitTrans()
    cur.execute(part2)

#####_____MIGRATION 10_____#####
def RunMigration10():
    cur = setupDB()

    part1 = "ALTER TABLE `Annotations_gff` ADD COLUMN `Public` TINYINT(1) UNSIGNED NOT NULL AFTER `FileName`"
    part2 = "ALTER TABLE `Annotations_gff` ADD COLUMN `User` VARCHAR(255) AFTER `Public`"

    part3 = "ALTER TABLE `DNAStorage_specimen` ADD COLUMN `User` VARCHAR(255) AFTER `Public`"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)

    from Annotations.models import GFF
    annotations = GFF.objects.all()
    for annotation in annotations:
        annotation.Public = True
        annotation.save()

#####_____MIGRATION 11_____#####
def RunMigration11():
    cur = setupDB()

    part1 = "DROP TABLE IF EXISTS `SkittleCore_statepacket`"
    part2 = "SET FOREIGN_KEY_CHECKS=0"
    part3 = "DROP TABLE IF EXISTS `SkittleCore_requestpacket`"
    part4 = "SET FOREIGN_KEY_CHECKS=1"
    part5 = "CREATE TABLE `SkittleCore_statepacket` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `specimen` varchar(200) NOT NULL, `chromosome` varchar(200) NOT NULL, `seq` longtext, `colorPalette` varchar(50) NOT NULL, `width` integer, `scale` integer, `start` integer, `length` integer NOT NULL, `requestedGraph` varchar(1), `searchStart` integer NOT NULL, `searchStop` integer NOT NULL)"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)

#####_____MIGRATION 12_____#####
def RunMigration12():
    cur = setupDB()

    part1 = "CREATE TABLE `SkittleCore_skittleuser` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `password` varchar(128) NOT NULL, `last_login` datetime NOT NULL, `is_superuser` bool NOT NULL, `Email` varchar(255) NOT NULL UNIQUE, `FirstName` varchar(255) NOT NULL, `LastName` varchar(255), `IsAdmin` bool NOT NULL, `IsActive` bool NOT NULL, `DateJoined` datetime NOT NULL, `State_id` integer UNIQUE)"
    part2 = "CREATE TABLE `SkittleCore_skittleuser_groups` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `skittleuser_id` integer NOT NULL, `group_id` integer NOT NULL, UNIQUE (`skittleuser_id`, `group_id`))"
    part3 = "CREATE TABLE `SkittleCore_skittleuser_user_permissions` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `skittleuser_id` integer NOT NULL,  `permission_id` integer NOT NULL, UNIQUE (`skittleuser_id`, `permission_id`))"
    part4 = "ALTER TABLE `SkittleCore_skittleuser_groups` ADD CONSTRAINT `group_id_refs_id_c5dbdda0` FOREIGN KEY (`group_id`) REFERENCES `auth_group` (`id`)"
    part5 = "ALTER TABLE `SkittleCore_skittleuser_user_permissions` ADD CONSTRAINT `permission_id_refs_id_af15d326` FOREIGN KEY (`permission_id`) REFERENCES `auth_permission` (`id`)"
    part6 = "ALTER TABLE `SkittleCore_skittleuser` ADD CONSTRAINT `State_id_refs_id_8b1369ee` FOREIGN KEY (`State_id`) REFERENCES `SkittleCore_statepacket` (`id`)"
    part7 = "ALTER TABLE `SkittleCore_skittleuser_groups` ADD CONSTRAINT `skittleuser_id_refs_id_4d6ae9f2` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"
    part8 = "ALTER TABLE `SkittleCore_skittleuser_user_permissions` ADD CONSTRAINT `skittleuser_id_refs_id_eb0d7075` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)
    cur.execute(part6)
    cur.execute(part7)
    cur.execute(part8)

#####_____MIGRATION 13_____#####
def RunMigration13():
    cur = setupDB()

    part1 = "DROP TABLE IF EXISTS `auth_user_groups`"
    part2 = "DROP TABLE IF EXISTS `auth_user_user_permissions`"
    part3 = "SET FOREIGN_KEY_CHECKS=0"
    part4 = "DROP TABLE IF EXISTS `auth_user`"
    part5 = "SET FOREIGN_KEY_CHECKS=1"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)

#####_____MIGRATION 14_____#####
def RunMigration14():
    cur = setupDB()

    partPre = "SET FOREIGN_KEY_CHECKS=0"
    part1 = "DROP TABLE IF EXISTS `Graphs_annotationdisplaystate`"
    partX = "DROP TABLE IF EXISTS `Graphs_sequenceentry`"
    part2 = "DROP TABLE IF EXISTS `Graphs_highlighterstate`"
    part3 = "DROP TABLE IF EXISTS `Graphs_nucleotidebiasstate`"
    part4 = "DROP TABLE IF EXISTS `Graphs_nucleotidedisplaystate`"
    part5 = "DROP TABLE IF EXISTS `Graphs_oligomerusagestate`"
    part6 = "DROP TABLE IF EXISTS `Graphs_repeatmapstate`"
    part7 = "DROP TABLE IF EXISTS `Graphs_threemerdetectorstate`"
    part8 = "DROP TABLE IF EXISTS `Graphs_similarityheatmapstate`"
    part9 = "DROP TABLE IF EXISTS `Graphs_oligomerusagestate`"
    part10 = "SET FOREIGN_KEY_CHECKS=1"

    cur.execute(partPre)
    cur.execute(part1)
    cur.execute(partX)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)
    cur.execute(part6)
    cur.execute(part7)
    cur.execute(part8)
    cur.execute(part9)
    cur.execute(part2)
    cur.execute(part10)

#####_____MIGRATION 15_____#####
def RunMigration15():
    cur = setupDB()

    part1 = "ALTER TABLE `DNAStorage_specimen` DROP COLUMN `User`"
    part2 = "ALTER TABLE `Annotations_gff` DROP COLUMN `User`"
    part3 = "CREATE TABLE `DNAStorage_specimen_User` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `specimen_id` integer NOT NULL, `skittleuser_id` integer NOT NULL, UNIQUE (`specimen_id`, `skittleuser_id`))"
    part4 = "ALTER TABLE `DNAStorage_specimen_User` ADD CONSTRAINT `specimen_id_refs_id_e50ef8d8` FOREIGN KEY (`specimen_id`) REFERENCES `DNAStorage_specimen` (`id`)"
    part5 = "ALTER TABLE `DNAStorage_specimen_User` ADD CONSTRAINT `skittleuser_id_refs_id_de15f914` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"
    part6 = "CREATE TABLE `Annotations_gff_User` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `gff_id` integer NOT NULL, `skittleuser_id` integer NOT NULL, UNIQUE (`gff_id`, `skittleuser_id`))"
    part7 = "ALTER TABLE `Annotations_gff` ADD CONSTRAINT `Specimen_id_refs_id_bed804f4` FOREIGN KEY (`Specimen_id`) REFERENCES `DNAStorage_specimen` (`id`)"
    part8 = "ALTER TABLE `Annotations_gff_User` ADD CONSTRAINT `gff_id_refs_id_2694d050` FOREIGN KEY (`gff_id`) REFERENCES `Annotations_gff` (`id`)"
    part9 = "ALTER TABLE `Annotations_gff_User` ADD CONSTRAINT `skittleuser_id_refs_id_7881899e` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)
    cur.execute(part6)
    cur.execute(part7)
    cur.execute(part8)
    cur.execute(part9)

#####_____MIGRATION 16_____#####
def RunMigration16():
    cur = setupDB()

    partPre = "SET FOREIGN_KEY_CHECKS=0"
    partTest = "ALTER TABLE `Annotations_gff` DROP FOREIGN KEY `Specimen_id_refs_id_bed804f4`"
    part1 = "ALTER TABLE `DNAStorage_specimen` DROP COLUMN `Public`"
    part2 = "DROP TABLE IF EXISTS `DNAStorage_specimen_user`"
    part3 = "SET FOREIGN_KEY_CHECKS=1"
    part4 = "ALTER TABLE `DNAStorage_fastafiles` ADD COLUMN `Public` TINYINT(1) UNSIGNED NOT NULL AFTER `Length`"
    part5 = "CREATE TABLE `DNAStorage_fastafiles_User` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `fastafiles_id` integer NOT NULL, `skittleuser_id` integer NOT NULL, UNIQUE (`fastafiles_id`, `skittleuser_id`))"
    part6 = "ALTER TABLE `DNAStorage_fastafiles_User` ADD CONSTRAINT `fastafiles_id_refs_id_56a60359` FOREIGN KEY (`fastafiles_id`) REFERENCES `DNAStorage_fastafiles` (`id`)"
    part7 = "ALTER TABLE `DNAStorage_fastafiles_User` ADD CONSTRAINT `skittleuser_id_refs_id_f38a8c68` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"

    cur.execute(partPre)
    cur.execute(partTest)
    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)
    cur.execute(part6)
    cur.execute(part7)

    from DNAStorage.models import FastaFiles
    chromosomes = FastaFiles.objects.all()
    for chromosome in chromosomes:
        chromosome.Public = True

        if chromosome.Chromosome == "bcbanner":
            chromosome.Public = False

        chromosome.save()


#####_____MIGRATION 17_____#####
def RunMigration17():
    cur = setupDB()

    part1 = "CREATE TABLE `DNAStorage_importprogress_User` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `importprogress_id` integer NOT NULL, `skittleuser_id` integer NOT NULL, UNIQUE (`importprogress_id`, `skittleuser_id`))"
    part2 = "CREATE TABLE `DNAStorage_importprogress` (`id` integer AUTO_INCREMENT NOT NULL PRIMARY KEY, `Specimen` varchar(255) NOT NULL, `FileName` varchar(255) NOT NULL, `Message` varchar(255), `IsWorking` bool NOT NULL, `Success` bool NOT NULL, `FastaFile_id` integer UNIQUE)"
    part3 = "ALTER TABLE `DNAStorage_importprogress` ADD CONSTRAINT `FastaFile_id_refs_id_b2920254` FOREIGN KEY (`FastaFile_id`) REFERENCES `DNAStorage_fastafiles` (`id`)"
    part4 = "ALTER TABLE `DNAStorage_importprogress_User` ADD CONSTRAINT `importprogress_id_refs_id_a5f0bb0c` FOREIGN KEY (`importprogress_id`) REFERENCES `DNAStorage_importprogress` (`id`)"
    part5 = "ALTER TABLE `DNAStorage_importprogress_User` ADD CONSTRAINT `skittleuser_id_refs_id_a0aa1864` FOREIGN KEY (`skittleuser_id`) REFERENCES `SkittleCore_skittleuser` (`id`)"

    cur.execute(part1)
    cur.execute(part2)
    cur.execute(part3)
    cur.execute(part4)
    cur.execute(part5)


#####_____MIGRATION 18_____#####
def RunMigration18():
    cur = setupDB()

    part1 = "ALTER TABLE `SkittleCore_skittleuser` ADD COLUMN `NewUser` TINYINT(1) NOT NULL AFTER `State_id`"

    cur.execute(part1)

    from SkittleCore.models import SkittleUser
    users = SkittleUser.objects.all()
    for user in users:
        user.NewUser = True
        user.save()


#####_____MIGRATION 19_____#####
def RunMigration19():
    cur = setupDB()

    part1 = "ALTER TABLE `SkittleCore_statepacket` ADD COLUMN `userId` integer NOT NULL UNIQUE AFTER `searchStop`"

    cur.execute(part1)


def commitTrans():
    from django.db import transaction

    transaction.enter_transaction_management()
    transaction.commit()
