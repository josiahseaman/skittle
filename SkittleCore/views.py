import json

from django.shortcuts import render, redirect
from django.http import HttpResponse
from django.views.decorators.cache import cache_control

from SkittleCore import GraphRequestHandler
from SkittleCore.models import RequestPacket
from SkittleCore.Graphs.models import *
from DNAStorage.StorageRequestHandler import *
from Annotations.StorageRequestHandler import GetAnnotationsChunk,GetAnnotationsList


def browse(request, genus="homo", species="sapiens", specimen="hg19", chromosome="chrY"):
    print "browse called as user " + str(request.user) + " who is authenticated:"+ str(request.user.is_authenticated())
    if IsUserForbidden(specimen, chromosome, request.user): #also checks existance
        return redirect('index')

    width = request.GET.get('width', 100)
    scale = request.GET.get('scale', 1)
    start = request.GET.get('start', 1)
    zoom = request.GET.get('zoom', 1)
    graphs = request.GET.get('graphs', "n")
    colorPalette = request.GET.get('colorPalette', 'Classic')
    fileLength = GetChromosomeLength(specimen,chromosome) 
    chromosomeList = GetRelatedChromosomes(specimen, request.user)
    context = {'availableGraphs':GraphRequestHandler.availableGraphs, 'availableAnnotations':GetAnnotationsList(specimen), "annotationStatus":json.dumps(GetAnnotationsList(specimen)), 'specimen':GetSpecimen(specimen),'chromosome':chromosome,'chromosomeList':chromosomeList ,'colorPalette':colorPalette,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs,"fileLength":fileLength,}
    return render(request, 'browse.html',context)

@cache_control(must_revalidate=False, max_age=3600)
def graph(request, genus="homo", species="sapiens", specimen="hg18", chromosome="chrY-sample"):
    if IsUserForbidden(specimen, chromosome, request.user): #also checks existance
        return HttpResponse(status=403)

    state = createRequestPacket(request, specimen, chromosome)
    graphSettings = None
    if state.requestedGraph == 'h':
        graphSettings = createHighlighterState(request, genus, species, specimen, chromosome)
        state.searchStart = int(request.GET.get('searchStart', 1))
        state.searchStop = int(request.GET.get('searchStop', 1))

    image_data = GraphRequestHandler.handleRequest(state, graphSettings)
    return HttpResponse(image_data, content_type="image/png")


def annotation(request, genus="homo", species="sapiens", specimen="hg18", chromosome="chrY-sample"):
    if (request.GET.get('start')):
        start = max(1, int(request.GET.get('start', 1)))
        annotations = request.GET.getlist('annotation', None)
        j = GetAnnotationsChunk(specimen, chromosome, start, annotations)
    else:
        j = json.dumps(GetAnnotationsList(specimen))
    return HttpResponse(j, content_type="application/json")


def state(request):
    j = "graphStatus = " + json.dumps(GraphRequestHandler.generateGraphListForServer())
    # j += "annotationSources = " + simplejson.dumps(StorageRequestHandler.getAnnotations())
    j += ";graphOrder = ['n','h','b','t','o','m','s'];"
    return HttpResponse(j,content_type="application/json")


def sequence(request, genus="homo", species="sapiens", specimen="hg18", chromosome="chrY-sample"):
    state = createRequestPacket(request, specimen, chromosome)
    import SequenceLogic

    searchStart = int(request.GET.get('queryStart', 10000))
    searchStop = int(request.GET.get('queryStop', 10010))
    seq = SequenceLogic.getSearchSequenceFromRequestPacket(state, searchStart, searchStop)
    return HttpResponse(seq)


def createRequestPacket(request, specimen, chromosome):
    state = RequestPacket()
    state.chromosome = chromosome
    state.specimen = specimen

    state.start = max(1, int(request.GET.get('start', 1)))
    state.width = max(12, int(request.GET.get('width', 100)))
    state.scale = max(1, int(request.GET.get('scale', 1)))
    state.requestedGraph = request.GET.get('graph', 'n')
    state.colorPalette = request.GET.get('colorPalette', 'Classic')
    return state


def createHighlighterState(request, genus, species, specimen, chromosome):
    graphSettings = HighlighterState()
    useRevComp = True if 'rev' in request.GET else False
    graphSettings.searchReverseComplement = useRevComp
    similarity = request.GET.get('sim', None)
    if similarity:
        graphSettings.minimumPercentage = float(similarity) / 100
    for i in range(50):
        searchSequence1 = request.GET.get('s' + str(i), None)
        if searchSequence1 is not None:
            print searchSequence1
            tmp = SequenceEntry()
            tmp.seq = searchSequence1.upper()
            color = request.GET.get('s' + str(i) + 'c', None)
            if color is not None:
                tmp.color = parseHexColor(color)

            graphSettings.targetSequenceEntries.append(tmp)
    print map(lambda x: x.seq, graphSettings.targetSequenceEntries)
    return graphSettings


def parseHexColor(colorString):
    r = int(colorString[:2], 16)
    g = int(colorString[2:4], 16)
    b = int(colorString[4:6], 16)
    return Color([r, g, b])
