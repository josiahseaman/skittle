from django.shortcuts import render
from django.http import HttpResponse
from django.utils import simplejson
from DNAStorage.models import Specimen, FastaFiles
from DNAStorage import StorageRequestHandler
from StorageRequestHandler import HandleUploadedFile
from django import forms


def index(request):
    specimens = Specimen.objects.filter(Public=True)
    chromosomes = FastaFiles.objects.order_by('Length').reverse()
    context = {'specimens': specimens, 'chromosomes': chromosomes}
    return render(request, 'index.html', context)


class uploadFileForm(forms.Form):
    availability = None  # TODO: Find out how to bring in the two radio buttons
    kingdomName = forms.CharField()  # TODO: Change these char fields to fix what ever Marshall does for the taxonomy selection
    className = forms.CharField()
    genusName = forms.CharField()
    speciesName = forms.CharField()
    specimenName = forms.CharField()
    genomeName = forms.CharField()
    source = forms.CharField()
    dateSequenced = forms.DateTimeField()
    description = forms.CharField()
    file = forms.FileField()

def Upload(request):
    status = None
    message =""
    if request.is_ajax() or request.method == 'POST':
        form = uploadFileForm(request.POST, request.FILES)
        if form.is_valid():
            filePath = HandleUploadedFile(request.FILES['file'])
        # handle the form data?
        status = "success"
        message = "Your files were uploaded successfully. Please Allow a few hours for processing."
    context = {'status':status,'message':message}
    return render(request, 'upload.html', context)


def taxonomy(request):
    json = "currentTaxonomy = " + simplejson.dumps(StorageRequestHandler.GetTreeList())
    return HttpResponse(json, content_type="application/json")