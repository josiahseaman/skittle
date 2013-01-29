from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from DNAStorage.models import Specimen,FastaFiles


def index(request):
    specimens = Specimen.objects.all
    chromosomes = FastaFiles.objects.all
    context = {'specimens':specimens,'chromosomes':chromosomes}
    return render(request, 'index.html',context)

