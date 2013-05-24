from django.shortcuts import render
from django.http import HttpResponse
from django.utils import simplejson
from DNAStorage.models import Specimen, FastaFiles
from DNAStorage import StorageRequestHandler


def index(request):
    specimens = Specimen.objects.filter(Public=True)
    chromosomes = FastaFiles.objects.order_by('Length').reverse()
    context = {'specimens': specimens, 'chromosomes': chromosomes}
    return render(request, 'index.html', context)


def upload(request):
    status = None
    message =""
    if request.is_ajax() or request.method == 'POST':
        # handle the form data?
        status = "success"
        message = "Your files were uploaded successfully. Please Allow a few hours for processing."
    context = {'status':status,'message':message}
    return render(request, 'upload.html', context)


def taxonomy(request):
    json = "currentTaxonomy = " + simplejson.dumps(StorageRequestHandler.GetTreeList())
    return HttpResponse(json, content_type="application/json")