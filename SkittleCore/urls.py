from django.conf.urls import patterns, url

from SkittleCore import views
import SkittleRequestHandler

urlpatterns = patterns('',
    url(r'^(?P<genome>\w+)/(?P<chromosome>\w+)/$', views.index, name='index'),
    url(r'^(?P<genome>\w+)_(?P<chromosome>\w+)_(?P<graph>\w)_(?P<start>\d+)_(?P<width>\d+)_(?P<scale>\d+)\.png$', views.graph, name='graph'),
    url(r'^$', views.index, name='index'),
)
