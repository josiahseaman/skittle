from SkittleCore.models import RequestPacket, StatePacket, SkittleUser

def SaveState(request, user):
    assert isinstance(request, RequestPacket)

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


def GetUserState(user):
    statePacket = StatePacket.objects.filter(userId=user.id)[:1]

    if statePacket:
        return statePacket[0]
    else:
        return None


def GetUserRequestFromState(user):
    statePacket = GetUserState(user)

    if statePacket:
        requestPacket = RequestPacket()

        requestPacket.specimen = statePacket.specimen
        requestPacket.chromosome = statePacket.chromosome
        requestPacket.seq = statePacket.seq
        requestPacket.colorPalette = statePacket.colorPalette
        requestPacket.width = statePacket.width
        requestPacket.scale = statePacket.scale
        requestPacket.start = statePacket.start
        requestPacket.length = statePacket.length
        requestPacket.requestedGraph = statePacket.requestedGraph
        requestPacket.searchStart = statePacket.searchStart
        requestPacket.searchStop = statePacket.searchStop

        return requestPacket
    else:
        return None
