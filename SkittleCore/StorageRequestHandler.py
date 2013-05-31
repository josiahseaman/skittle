from SkittleCore.models import RequestPacket, StatePacket, SkittleUser

def SaveState(request, user):
    assert isinstance(request, RequestPacket)
    assert isinstance(user, SkittleUser)

    statePacket = StatePacket.objects.filter(userId=user.id)[:1]

    if not statePacket:
        statePacket = StatePacket(userId=user.id)
    else:
        statePacket = statePacket[0]

    statePacket.specimen = request.specimen
    statePacket.chromosome = request.chromosome
    statePacket.seq = request.seq
    statePacket.colorPalette = request.colorPalette
    statePacket.width = request.width
    statePacket.scale = request.scale
    statePacket.start = request.start
    statePacket.length = request.length
    statePacket.requestedGraph = request.requestedGraph
    statePacket.searchStart = request.searchStart
    statePacket.searchStop = request.searchStop

    statePacket.save()

    user.State = statePacket
    user.save()
