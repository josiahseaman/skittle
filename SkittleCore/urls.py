from django.conf.urls import patterns, url

from SkittleCore import views
import SkittleRequestHandler

urlpatterns = patterns('',
    url(r'^(?P<genome>\w+)/(?P<chromosome>\w+)/$', views.index, name='index'),
    url(r'^(?P<genome>\w+)_(?P<chromosome>[a-zA-Z\-]+)_(?P<graph>\w)_(?P<start>\d+)_(?P<scale>\d+)_(?P<width>\d+)\.png$', views.graph, name='graph'),
    url(r'^data\.png$', views.graph, name='graph'),
    url(r'^$', views.index, name='index'),
)
