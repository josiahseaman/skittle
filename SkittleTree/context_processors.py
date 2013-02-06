from django.conf import settings 

def global_vars(context):
    return {'PRODUCTION': settings.PRODUCTION, 'DEBUG':settings.DEBUG}