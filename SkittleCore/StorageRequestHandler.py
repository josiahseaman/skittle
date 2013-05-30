from django.conf import settings

def GetUser(userId):
    user = settings.AUTH_USER_MODEL.objects.filter(id=userId)[:1]

    if user:
        return user[0]
    else:
        return None
