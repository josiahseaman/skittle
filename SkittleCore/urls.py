from django.conf.urls import patterns, url

from SkittleCore import views
import SkittleRequestHandler

urlpatterns = patterns('',
    url(r'^(?P<genome>\w+)/(?P<chromosome>\w+)/$', views.index, name='index'),
    url(r'^data\.png$', views.graph, name='graph'),
    url(r'^state\.json$', views.state, name='state'),
    url(r'^$', views.index, name='index'),
)
