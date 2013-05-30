from models import SkittleUser, StatePacket

def GetUser(userId):
    user = SkittleUser.objects.filter(id=userId)[:1]

    if user:
        return user[0]
    else:
        return None
