from django.conf.urls import patterns, include, url
from django.views.generic import RedirectView, TemplateView

# Uncomment the next two lines to enable the admin:
from django.contrib import admin

admin.autodiscover()

urlpatterns = patterns('',
   # Examples:
   url(r'^$', 'SkittleTree.views.home', name='home'),
   url(r'^browse/', include('SkittleCore.urls')),
   url(r'^discover/', include('DNAStorage.urls')),
   url(r'^23andMe/', RedirectView.as_view(url='/browse/homo/sapiens/hg19/chrY/?graphs=an')),
   # url(r'^$', include('SkittleCore.urls')),
   url(r'^googlef44684f34c2340e5.html$', 'SkittleTree.views.google'),

   # Uncomment the admin/doc line below to enable admin documentation:
   # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

   # Uncomment the next line to enable the admin:
   url(r'^admin/', include(admin.site.urls)),
   url(r'^sendFeedback$', 'SkittleTree.views.feedbackSend'),
   (r'^learn/?$', 'SkittleTree.views.learn'),
   # (r'^learn/?$', RedirectView.as_view(url='/browse/homo/sapiens/hg19/chrY/?graphs=bn&start=1468365&scale=1&width=105&annotation=gencode#learn')),
   (r'^card/?$', RedirectView.as_view(url='/browse/businesscard/bcbanner/?graphs=abn&start=1&scale=1&width=296')),
   # (r'^card/?$', TemplateView.as_view(template_name='bizcard.html')),
   (r'^try/?$', 'SkittleTree.views.loggedInRedirect'),
   url(r'^accounts/login/?$', 'django.contrib.auth.views.login', {'template_name': 'login.html'}, name='login'),
   url(r'^accounts/logout/?$', 'django.contrib.auth.views.logout_then_login', name='logout'),
   url(r'^accounts/resetpassword/$', 'django.contrib.auth.views.password_reset', {'template_name': 'password_reset_form.html', 'email_template_name': 'password_reset_email.html'}, name='password_reset'),
   url(r'^accounts/passwordreset/$', 'django.contrib.auth.views.password_reset_done', {'template_name': 'password_reset_done.html'}, name='password_reset_done'),
   url(r'^accounts/setpassword/(?P<uidb36>\d+)/(?P<token>[\d\w-]+)$', 'django.contrib.auth.views.password_reset_confirm', {'template_name': 'password_reset_confirm.html'}, name='password_reset_confirm'),
   url(r'^accounts/resetcomplete/$', 'django.contrib.auth.views.password_reset_complete', {'template_name': 'password_reset_complete.html'}, name='password_reset_complete'),
   url(r'^accounts/new/?$', 'SkittleTree.views.createUser'),
)
