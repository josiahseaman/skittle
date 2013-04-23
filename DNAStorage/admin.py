from django.contrib import admin

from DNAStorage.models import Specimen, FastaFiles


class SpecimenAdmin(admin.ModelAdmin):
    list_display = ('ExtendedName', 'Name', 'DatePublished', 'GenomeLength')


admin.site.register(Specimen, SpecimenAdmin)
admin.site.register(FastaFiles)