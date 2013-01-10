from django.conf.urls import patterns, url

from SkittleCore import views

urlpatterns = patterns('',
    url(r'^(?P<fasta>\w+)/$', views.index, name='index'),
    url(r'^$', views.index, name='index'),
)
