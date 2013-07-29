from django.shortcuts import render
from django.http import HttpResponse
from django.utils import simplejson
from django import forms
from django.contrib.auth.decorators import login_required
from itertools import chain

from django.db.models import Q

from DNAStorage.models import Specimen, FastaFiles
from DNAStorage import StorageRequestHandler


def index(request):
    if request.user.is_authenticated(): chromosomes = FastaFiles.objects.filter(Q(Public=True) | Q(User=request.user))
    else: chromosomes = FastaFiles.objects.filter(Public=True)
    specimenNames = chromosomes.values('Specimen__Name').distinct()
    specimenNames = [n.values()[0] for n in specimenNames if n.values()[0] != "businesscard"]
    specimens = Specimen.objects.filter(Name__in=specimenNames)

    if request.GET.get('user'):
        specimens = specimens.filter(fastafiles__User__email=request.GET.get('user')).distinct() #only return uploads from specified user
    if request.GET.get('kingdom'):
        specimens = specimens.filter(Kingdom__iexact=request.GET.get('kingdom'))
    if request.GET.get('class'):
        specimens = specimens.filter(Class__iexact=request.GET.get('class'))
    if request.GET.get('genus'):
        specimens = specimens.filter(Genus__iexact=request.GET.get('genus'))
    if request.GET.get('species'):
        specimens = specimens.filter(Species__iexact=request.GET.get('species'))


    tree = StorageRequestHandler.GetTreeList(request.user)
    context = {'specimens': specimens, 'tree': tree}
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

@login_required
def Upload(request):
    status = None
    message =""
    if request.is_ajax() or request.method == 'POST':
        # form = uploadFileForm(request.POST, request.FILES)
        if request.method == 'POST':
            genomeInfo = {
                            'kingdom':request.POST.get('Kingdom',u''),
                            'class':request.POST.get('Class',None),
                            'genus':request.POST.get('Genus',None),
                            'species':request.POST.get('Species',None),
                            'specimen':request.POST.get('specimenName',"Unknown"),
                            'genomeName':request.POST.get('genomeName',None),
                            'source':request.POST.get('source',None),
                            'dateSequenced':request.POST.get('dateSequenced',None),
                            'description':request.POST.get('description',None),
                            'isPublic':request.POST.get('isPublic',False),
                            'chromosome':request.POST.get('chromosomeName',None)
                        }
            filePath = StorageRequestHandler.HandleUploadedFile(request.FILES['file'],genomeInfo,request.user)
        uploads = StorageRequestHandler.GetUserImports(request.user).distinct()
        return render(request, 'uploadStatus.json', {'uploads':uploads}, content_type="application/json")
    context = {'status':status,'message':message,'existingSpecimens':StorageRequestHandler.GetTreeList(request.user)}
    return render(request, 'upload.html', context)


def taxonomy(request):
    json = "currentTaxonomy = " + simplejson.dumps(StorageRequestHandler.GetTreeList())
    return HttpResponse(json, content_type="application/json")