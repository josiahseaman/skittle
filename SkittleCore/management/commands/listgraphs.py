from django.core.management.base import BaseCommand
import SkittleCore.GraphRequestHandler as grh


class Command(BaseCommand):
    args = ''
    help = 'List all registered graphs on the server with kittens.'

    def handle(self, *args, **options):
        self.stdout.write(str(grh.GetRegisteredGraphsSymbols()) + "\n")
        