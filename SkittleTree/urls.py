from django.conf.urls import patterns, include, url
from django.conf import settings
from django.views.generic.simple import redirect_to

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    url(r'^$', 'SkittleTree.views.home', name='home'),
    url(r'^browse/', include('SkittleCore.urls')),
    url(r'^discover/', include('DNAStorage.urls')),
    (r'^$', redirect_to, {'url': '/browse/'}),
    # url(r'^$', include('SkittleCore.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
    url(r'^sendFeedback$', 'SkittleTree.views.feedbackSend')
)
