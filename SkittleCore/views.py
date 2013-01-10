from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict

def index(request, fasta="test"):
    # return render_to_response('index.html')
    width = request.GET.get('width',100)
    scale = request.GET.get('scale',1)
    start = request.GET.get('start',1)
    zoom = request.GET.get('zoom',1)
    graphs = request.GET.get('graphs',"n")
    context = {'fasta':fasta,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs}
    return render(request, 'index.html',context)
