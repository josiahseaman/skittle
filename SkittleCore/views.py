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
from Annotations.StorageRequestHandler import GetAnnotationsChunk
# import json

def browse(request, genus="homo",species="sapiens", specimen="hg18",chromosome="chrY-sample"):
    width = request.GET.get('width',100)
    scale = request.GET.get('scale',1)
    start = request.GET.get('start',1)
    zoom = request.GET.get('zoom',1)
    graphs = request.GET.get('graphs',"n")
    colorPalette = request.GET.get('colorPalette','Classic')
    selectionStart = request.GET.get('searchStart',1)
    selectionEnd = request.GET.get('searchStop',1)
    if 'h' in graphs:
        highlighterState = simplejson.dumps(createHighlighterState(request,genus,species,specimen,chromosome).__dict__)
    else: highlighterState = "undefined"

    fileLength = GetChromosomeLength(specimen,chromosome) 
    context = {'availableGraphs':GraphRequestHandler.availableGraphs,'specimen':specimen,'chromosome':chromosome,'colorPalette':colorPalette,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs,"fileLength":fileLength,"highlighterState":highlighterState}
    return render(request, 'browse.html',context)

@cache_control(must_revalidate=False, max_age=3600)
def graph(request, genus="homo",species="sapiens", specimen="hg18",chromosome="chrY-sample"):
    state = createRequestPacket(request, specimen, chromosome)
    graphSettings = None
    if state.requestedGraph == 'h':
        graphSettings = createHighlighterState(request,genus,species,specimen,chromosome)
        state.searchStart = int(request.GET.get('searchStart',1))
        state.searchStop = int(request.GET.get('searchStop',1))

    image_data = handleRequest(state, graphSettings)
    return HttpResponse(image_data, content_type="image/png")

def annotation(request, genus="homo",species="sapiens", specimen="hg18",chromosome="chrY-sample"):
    start = max(1,int(request.GET.get('start',1)))
    json = GetAnnotationsChunk(specimen,chromosome,start)
    return HttpResponse(json,content_type="application/json")

def state(request):
    json = "graphStatus = " + simplejson.dumps(GraphRequestHandler.generateGraphListForServer())
    # json = "annotationSources = " + simplejson.dumps(StorageRequestHandler.getAnnotations())
    json += ";graphOrder = ['a','n','h','b','t','o','m','s'];"
    return HttpResponse(json,content_type="application/json")

def sequence(request, genus="homo",species="sapiens", specimen="hg18",chromosome="chrY-sample"):
    state = createRequestPacket(request, specimen, chromosome)
    import SequenceLogic
    searchStart = int(request.GET.get('queryStart',10000))
    searchStop  =  int(request.GET.get('queryStop',10010))
    seq = SequenceLogic.getSearchSequenceFromRequestPacket(state, searchStart, searchStop)
    return HttpResponse(seq)

def createRequestPacket(request, specimen, chromosome):
    state = RequestPacket()
    state.chromosome = chromosome
    state.specimen = specimen

    state.start = max(1,int(request.GET.get('start',1)))
    state.width = max(12,int(request.GET.get('width',100)))
    state.scale = max(1,int(request.GET.get('scale',1)))
    state.requestedGraph = request.GET.get('graph','n')
    state.colorPalette = request.GET.get('colorPalette','Classic')
    return state

def createHighlighterState(request,genus,species,specimen,chromosome):
    graphSettings = HighlighterState()
    useRevComp = True if request.GET.get('rev') else False
    graphSettings.searchReverseComplement = useRevComp
    similarity = request.GET.get('sim', None)
    if similarity:
        graphSettings.minimumPercentage = float(similarity)/100
    for i in range(50):
        searchSequence1 = request.GET.get('s'+str(i), None)
        if searchSequence1 is not None:
            print searchSequence1
            tmp = SequenceEntry()
            tmp.seq = searchSequence1.upper()
            color = request.GET.get('s'+str(i)+'c', None)
            if color is not None:
                tmp.color = parseHexColor(color)
                
            graphSettings.targetSequenceEntries.append(tmp)
    print map(lambda x: x.seq, graphSettings.targetSequenceEntries)
    return graphSettings

def parseHexColor(colorString):
    r = int(colorString[:2], 16)
    g = int(colorString[2:4], 16)
    b = int(colorString[4:6], 16)
    return (r, g, b)
