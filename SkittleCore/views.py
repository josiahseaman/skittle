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
    selectionStart = request.GET.get('searchStart',1)
    selectionEnd = request.GET.get('searchStop',1)

    fileLength = GetChromosomeLength(specimen,chromosome) 
    context = {'availableGraphs':GraphRequestHandler.availableGraphs,'specimen':specimen,'chromosome':chromosome,'colorPalette':colorPalette,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs,"fileLength":fileLength,'selectionStart':selectionStart,'selectionEnd':selectionEnd}
    return render(request, 'browse.html',context)

@cache_control(must_revalidate=False, max_age=3600)
def graph(request, specimen="hg18",chromosome="chrY-sample"):
    state = RequestPacket()
    state.chromosome = chromosome
    state.specimen = specimen

    state.start = max(1,int(request.GET.get('start',1)))
    state.width = max(12,int(request.GET.get('width',100)))
    state.scale = max(1,int(request.GET.get('scale',1)))
    state.requestedGraph = request.GET.get('graph','n')
    state.colorPalette = request.GET.get('colorPalette','Classic')
    if state.requestedGraph == 'h':
    	state.searchStart = int(request.GET.get('searchStart',1))
    	state.searchStop = int(request.GET.get('searchStop',1))

    image_data = handleRequest(state)
    # image_data = open("/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/assets/n-display.png", "rb").read()
    return HttpResponse(image_data, content_type="image/png")

def annotation(request, specimen="hg18",chromosome="chrY-sample"):
    state.start = max(1,int(request.GET.get('start',1)))
    json = '{"' + str(state.start) + '":{"1":["ensembl","chromosome",1,1567,0,".",null,"ID=9;Name=chromosome:AGPv2:9:1:156750706:1"],"76118":["ensembl","chromosome",3013,5413,0,".",null,"ID=1;Name=chromosome:AGPv2:1:1:301354135:1"],"232012":["ensembl","chromosome",24147,25504,0,".",null,"ID=4;Name=chromosome:AGPv2:4:1:241473504:1"],"337780":["ensembl","chromosome",21787,28520,0,".",null,"ID=5;Name=chromosome:AGPv2:5:1:217872852:1"],"454891":["ensembl","chromosome",23706,28873,0,".",null,"ID=2;Name=chromosome:AGPv2:2:1:237068873:1"],"571481":["ensembl","chromosome",1,33214,34174,0,".",null,"ID=3;Name=chromosome:AGPv2:3:1:232140174:1"],"677560":["ensembl","chromosome",41691,44353,0,".",null,"ID=6;Name=chromosome:AGPv2:6:1:169174353:1"],"760463":["ensembl","chromosome",51757,93759,0,".",null,"ID=8;Name=chromosome:AGPv2:8:1:175793759:1"],"848142":["ensembl","chromosome",51767,64762,0,".",null,"ID=7;Name=chromosome:AGPv2:7:1:176764762:1"],"926976":["ensembl","chromosome",51501,59435,0,".",null,"ID=10;Name=chromosome:AGPv2:10:1:150189435:1"],"992796":["ensembl","chromosome",57140,58151,0,".",null,"ID=UNKNOWN;Name=chromosome:AGPv2:UNKNOWN:1:7140151:1"],"993569":["ensembl","chromosome",61403,61484,0,".",null,"ID=Pt;Name=chromosome:AGPv2:Pt:1:140384:1"],"993818":["ensembl","chromosome",65696,65730,0,".",null,"ID=Mt;Name=chromosome:AGPv2:Mt:1:569630:1"]}}'
    # json = '''{"1":["ensembl","chromosome",1,1567,0,".",null,"ID=9;Name=chromosome:AGPv2:9:1:156750706:1"]}''' 
    return HttpResponse(json,content_type="application/json")

def state(request):
    json = '''graphStatus = {
        "a":{name:"Annotations",visible:false,isRasterable:true},
        "n":{name:"Nucleotide Display",visible:true,isRasterable:true},
        "b":{name:"Nucleotide Bias",visible:false,isRasterable:false},
        "m":{name:"Repeat Map",visible:false,isRasterable:false}
    }''' 
    json = "graphStatus = " + simplejson.dumps(GraphRequestHandler.generateGraphListForServer())
    json += ";graphOrder = ['a','n','p','h','b','t','o','m','s'];"
    return HttpResponse(json,content_type="application/json")

