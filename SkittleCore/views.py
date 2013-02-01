from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from django.views.decorators.cache import cache_control
from django.utils import simplejson
from SkittleCore import GraphRequestHandler
from SkittleCore.GraphRequestHandler import handleRequest
from SkittleCore.models import RequestPacket
from SkittleCore.Graphs.models import *
from DNAStorage.StorageRequestHandler import GetChromosomeLength
# import json

def browse(request, specimen="hg18",chromosome="chrY-sample"):
    width = request.GET.get('width',100)
    scale = request.GET.get('scale',1)
    start = request.GET.get('start',1)
    zoom = request.GET.get('zoom',1)
    graphs = request.GET.get('graphs',"n")
    colorPalette = request.GET.get('colorPalette','Classic')

    fileLength = GetChromosomeLength(specimen,chromosome) 
    context = {'availableGraphs':GraphRequestHandler.availableGraphs,'specimen':specimen,'chromosome':chromosome,'colorPalette':colorPalette,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs,"fileLength":fileLength}
    return render(request, 'browse.html',context)

@cache_control(must_revalidate=False, max_age=3600)
def graph(request, specimen="hg18",chromosome="chrY-sample"):
    state = RequestPacket()
    state.chromosome = chromosome
    state.specimen = specimen

    state.start = int(request.GET.get('start',1))
    state.width = int(request.GET.get('width',100))
    state.scale = int(request.GET.get('scale',1))
    state.requestedGraph = request.GET.get('graph','n')
    state.colorPalette = request.GET.get('colorPalette','Classic')
    if state.requestedGraph == 'h':
    	state.searchStart = int(request.GET.get('searchStart',1))
    	state.searchStop = int(request.GET.get('searchStop',1))

    image_data = handleRequest(state)
    # image_data = open("/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/assets/n-display.png", "rb").read()
    return HttpResponse(image_data, content_type="image/png")

def annotation(request, specimen="hg18",chromosome="chrY-sample"):
    json = '''annotation = [
        {from:1,to:220,name:"annotation one"},
        {from:1209,to:3520,name:"annotation two"},
        {from:2039,to:2800,name:"annotation three"},
        {from:12039,to:12800,name:"annotation four"},
    ]''' 
    return HttpResponse(json,content_type="application/json")
def state(request):
    json = '''graphStatus = {
        "a":{name:"Annotations",visible:false,isRasterable:true},
        "n":{name:"Nucleotide Display",visible:true,isRasterable:true},
        "b":{name:"Nucleotide Bias",visible:false,isRasterable:false},
        "m":{name:"Repeat Map",visible:false,isRasterable:false}
    }''' 
    json = "graphStatus = " + simplejson.dumps(GraphRequestHandler.generateGraphListForServer())
    json += ";graphOrder = ['a','n','h','b','t','o','m','s'];"
    return HttpResponse(json,content_type="application/json")

