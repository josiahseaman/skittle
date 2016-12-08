import os

from django.core.management.base import BaseCommand
from django.core.management import call_command
from DNAStorage.StorageRequestHandler import GetPngFilePath
from DNAStorage.models import ImportProgress, ImageFiles


class FakeRequest:
    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)

class Command(BaseCommand):
    args = ''
    help = 'Checks all ImageFiles and ensures that they have the proper files on the disk. Also clears process queues.'

    def handle(self, *args, **options):
        self.stdout.write("Purging the current process queue!\n")
        call_command('purgeprocessqueue')

        self.stdout.write("Purging all current imports!\n")
        ImportProgress.objects.all().delete()

        self.stdout.write("Checking for invalid png files!\n")
        ImageFiles.objects.all().update(IsInRamDisk=False)
        for imagefile in ImageFiles.objects.all():
            fake_request = FakeRequest(specimen=imagefile.FastaFile.Specimen.Name,
                                       chromosome=imagefile.FastaFile.Chromosome,
                                       requestedGraph=imagefile.GraphType,
                                       start=imagefile.Start,
                                       scale=imagefile.Scale,
                                       width=imagefile.CharactersPerLine)
            if not os.path.isfile(GetPngFilePath(fake_request)):
                self.stdout.write("Deleting %s %s %s %s!\n" % (fake_request.specimen, fake_request.chromosome, fake_request.requestedGraph, fake_request.start))
                imagefile.delete()

        self.stdout.write("Done cleaning ImageFiles!\n")
