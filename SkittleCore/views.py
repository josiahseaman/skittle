from django.shortcuts import render_to_response
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse

def index(request):
    return render_to_response('index.html')
