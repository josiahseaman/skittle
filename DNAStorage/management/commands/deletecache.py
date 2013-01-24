from django.core.management.base import BaseCommand, CommandError
from SkittleCore.GraphRequestHandler import generateGraphListForServer
from DNAStorage.StorageRequestHandler import DeleteCache

class Command(BaseCommand):
    args = '<graph_type graph_type ...>'
    help = 'Wipes cache associated with the given graph type.'
    
    def handle(self, *args, **options):
        if "all" in args:
            all = ['a', 'b', 'h', 'm', 'o', 'n', 's', 't']
        else:
            all = args
            
        for graph in all:
            try:
                if graph in generateGraphListForServer():
                    self.stdout.write("Wiping all PNGs and image database entries to do with graph type " + graph + "\n")
                else:
                    raise TypeError('Graph type "%s" does not exist!' % graph)
            except Exception as ex: #Graph type does not exist
                raise CommandError(ex)
                
            #Wipe the cache
            DeleteCache(graph)
        
        self.stdout.write("Done wiping cache for all given graphs!\n")
        