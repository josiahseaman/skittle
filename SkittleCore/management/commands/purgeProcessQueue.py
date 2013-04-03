from django.core.management.base import BaseCommand, CommandError
from SkittleCore.models import ProcessQueue

class Command(BaseCommand):
    args = ''
    help = 'Purges all processes in the process queue database'
    
    def handle(self, *args, **options):
        self.stdout.write("Purging all processes in the process queue database...\n")
        
        processes = ProcessQueue.objects.all().delete()
        
        self.stdout.write("Purged all processes!\n")
        