from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from django.utils import simplejson
from DNAStorage.models import Specimen,FastaFiles
from DNAStorage import StorageRequestHandler


def index(request):
    specimens = Specimen.objects.all
    chromosomes = FastaFiles.objects.all
    context = {'specimens':specimens,'chromosomes':chromosomes}
    return render(request, 'index.html',context)

def upload(request):
	context = {}
	return render(request, 'upload.html', context)

def taxonomy(request):
    json = "currentTaxonomy = " + simplejson.dumps(StorageRequestHandler.GetTreeList())
    return HttpResponse(json,content_type="application/json")