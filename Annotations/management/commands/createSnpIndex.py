from django.core.management.base import BaseCommand
import Annotations.import_snp


class Command(BaseCommand):
    args = ''
    help = 'Creates the SNP index database for reading 23andMe user info'

    def handle(self, *args, **options):
        self.stdout.write("Creating SNP database once and for all...\n")

        Annotations.import_snp.createSnpIndex()

        self.stdout.write("Stuff happened!\n")
        