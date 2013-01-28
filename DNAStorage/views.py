from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from DNAStorage.models import FastaFiles


def index(request):
    return render(request, 'index.html')

