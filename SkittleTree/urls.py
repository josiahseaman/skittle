from django.conf.urls import patterns, include, url
from django.conf import settings
from django.views.generic import RedirectView

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    url(r'^$', 'SkittleTree.views.home', name='home'),
    url(r'^browse/', include('SkittleCore.urls')),
    url(r'^discover/', include('DNAStorage.urls')),
    (r'^$', RedirectView.as_view(url= '/browse/')),
    # url(r'^$', include('SkittleCore.urls')),
    url(r'^googlef44684f34c2340e5.html$', 'SkittleTree.views.google'),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
    url(r'^sendFeedback$', 'SkittleTree.views.feedbackSend'),
    (r'^learn/?$', RedirectView.as_view(url= '/browse/hg19/chrY/?graphs=abn&start=1468365&scale=1&width=105#learn')),
    (r'^card/?$', RedirectView.as_view(url= '/browse/businesscard/bcbanner/?graphs=abn&start=1&scale=1&width=296')),
)
