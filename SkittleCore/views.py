from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from SkittleCore.SkittleRequestHandler import handleRequest
from SkittleCore.models import StatePacket

def index(request, genome="",chromosome=""):
    width = request.GET.get('width',100)
    scale = request.GET.get('scale',1)
    start = request.GET.get('start',1)
    zoom = request.GET.get('zoom',1)
    graphs = request.GET.get('graphs',"n")
    context = {'genome':genome,'width':width, "scale":scale,"start":start,"zoom":zoom,"graphs":graphs}
    return render(request, 'index.html',context)

def graph(request,genome="Animalia_Mammalia_Homo_Sapiens_hg19",chromosome="chrY-sample",graph="n",start=1,width=1,scale=1):
	state = StatePacket()
	state.genome = "Animalia/Mammalia/Homo/Sapiens/" + genome
	state.chromosome = chromosome
	state.graph = graph
	state.start = start
	state.width = width
	state.scale = scale
	state.requestedGraph = graph
	image_data = handleRequest(state)
	# image_data = open("/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/assets/n-display.png", "rb").read()
	return HttpResponse(image_data, mimetype="image/png")
