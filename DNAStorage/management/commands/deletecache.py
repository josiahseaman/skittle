from django.core.management.base import BaseCommand, CommandError
from SkittleCore.GraphRequestHandler import GetRegisteredGraphsSymbols
from DNAStorage.StorageRequestHandler import DeleteCache, GetSpecimen, GetRelatedChromosomes
from optparse import make_option

class Command(BaseCommand):
    option_list = BaseCommand.option_list \
                  + (make_option('-e', '--specimen', action='store', type='string', dest='specimen', help='The specimen name that you want to delete the graphs for.'),) \
                  + (make_option('-c', '--chromosome', action='store', type='string', dest='chromosome', help='The chromosome that you want to delete the graphs for.'),) \
                  + (make_option('-s', '--start', action='store', type='int', dest='start', help='The start index that you want to delete the graphs for.'),)
    args = '[graph_type graph_type ...] -e|--specimen=[specimen] -c|--chromosome=[chromosome] -s|--start=[start]'
    help = 'Wipes cache associated with the given graph type.'

    def handle(self, *args, **options):
        if "all" in args:
            all = GetRegisteredGraphsSymbols()
        else:
            all = args

        for graph in all:
            try:
                if not graph in GetRegisteredGraphsSymbols():
                    raise TypeError('Graph type "%s" does not exist!' % graph)
                if options['specimen'] and not GetSpecimen(options['specimen']):
                    raise TypeError('Specimen "%s" does not exist!' % options['specimen'])
                if options['specimen'] and options['chromosome'] and not options['chromosome'] in GetRelatedChromosomes(GetSpecimen(options['specimen'])):
                    raise TypeError('Chromosome "%s" does not exist in given specimen!' % options['chromosome'])

                self.stdout.write("Wiping all PNGs and Image Database entries to do with graph type " + graph + " " + (options['specimen'] or "") + " " + (options['chromosome'] or "") + " " + str(options['start']) + "\n")
            except Exception as ex: #Graph type does not exist
                raise CommandError(ex)

            #Wipe the cache
            DeleteCache(graph, options['specimen'], options['chromosome'], options['start'])

        self.stdout.write("Done wiping cache for all given graphs!\n")
        