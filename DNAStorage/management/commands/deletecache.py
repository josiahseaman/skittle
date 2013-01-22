from django.core.management.base import BaseCommand, CommandError
from SkittleCore.GraphRequestHandler import generateGraphListForServer

class Command(BaseCommand):
    args = '<graph_type graph_type ...>'
    help = 'Wipes cache associated with the given graph type.'
    
    def handle(self, *args, **options):
        for graph in args:
            try:
                if graph in generateGraphListForServer:
                    self.stdout.write("Wiping all PNGs and image database entries to do with graph type " + graph)
                else:
                    raise TypeError('Graph type "%s" does not exist!' % graph)
            except Exception as ex: #Graph type does not exist
                raise CommandError(ex.strerror)
                
            #Wipe the cache
        
        self.stdout.write("Done wiping cache for all given graphs!")
        