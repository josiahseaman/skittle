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
    # kingdomName = forms.CharField()  # TODO: Change these char fields to fix what ever Marshall does for the taxonomy selection
    # className = forms.CharField()
    # genusName = forms.CharField()
    # speciesName = forms.CharField()
    specimenName = forms.CharField()
    genomeName = forms.CharField(required=False)
    source = forms.CharField(required=False)
    dateSequenced = forms.DateTimeField(required=False)
    description = forms.CharField(required=False)
    file = forms.FileField()

def Upload(request):
    status = None
    message =""
    if request.is_ajax() or request.method == 'POST':
        # form = uploadFileForm(request.POST, request.FILES)
        print request.POST
        genomeInfo = {
                        'kingdom':request.POST.get('kingdom',None),
                        'class':request.POST.get('className',None),
                        'genus':request.POST.get('genusName',None),
                        'species':request.POST.get('species',None),
                        'specimen':request.POST.get('specimenName',None),
                        'genomeName':request.POST.get('genomeName',None),
                        'source':request.POST.get('source',None),
                        'dateSequenced':request.POST.get('dateSequenced',None),
                        'description':request.POST.get('description',None),
                        'isPublic':request.POST.get('isPublic',True)
                    }
        # if form.is_valid():
        filePath = HandleUploadedFile(request.FILES['file'],genomeInfo)
        # handle the form data?
        status = "success"
        message = "Your files were uploaded successfully. Please Allow a few hours for processing."
    context = {'status':status,'message':message}
    return render(request, 'upload.html', context)


def taxonomy(request):
    json = "currentTaxonomy = " + simplejson.dumps(StorageRequestHandler.GetTreeList())
    return HttpResponse(json, content_type="application/json")