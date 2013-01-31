from DNAStorage.models import Specimen
from django.contrib import admin


class SpecimenAdmin(admin.ModelAdmin):
    list_display = ('ExtendedName','Name','DatePublished','GenomeLength')

admin.site.register(Specimen,SpecimenAdmin)