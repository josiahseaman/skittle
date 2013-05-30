import models as core

def GetUser(userId):
    user = core.SkittleUser.objects.filter(id=userId)[:1]

    if user:
        return user[0]
    else:
        return None
