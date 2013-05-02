from django.core.management.base import BaseCommand, CommandError
from Utilities.SkittleUtils import GetRoundedIndex


class Command(BaseCommand):
    args = ''
    help = 'Tells you the actual chunk starting value for any given start index.'

    def handle(self, *args, **options):
        try:
            if args[0]:
                start = int(args[0])
            else:
                raise TypeError('Invalid starting index!')

            roundedIndex = GetRoundedIndex(start)

            self.stdout.write("The Rounded Chunk Starting Value is: " + str(roundedIndex) + "\n")
        except Exception as ex:
            raise CommandError(ex)
        