from django.conf.urls import patterns, url

from DNAStorage import views

urlpatterns = patterns('',
               url(r'^$', views.index, name='index'),
               url(r'^upload$', views.Upload, name='upload'),
               url(r'^taxonomy.json$', views.taxonomy, name='taxonomy'),
)
