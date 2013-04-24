# Django settings for SkittleTree project.
import os
import socket

PRODUCTION = True
DEBUG = True
TEMPLATE_DEBUG = DEBUG

#--------------------

CHUNK_SIZE = 65536

#--------------------

if PRODUCTION and not socket.gethostname().startswith('nyx'):
    SKITTLE_TREE_LOC = "/var/www/skittle/"

    SKITTLE_TREE_URL = "http://dnaskittle.com/"
elif socket.gethostname().startswith('nyx'):
    SKITTLE_TREE_LOC = "/var/www/skittle-development/"

    SKITTLE_TREE_URL = "http://skittle.newlinetechnicalinnovations.com/"
else:
    SKITTLE_TREE_LOC = os.getcwd().replace("\\", "/") + "/"
    SKITTLE_TREE_URL = "/"

ADMINS = (
    ('Josiah Seaman', 'josiah@newlinetechnicalinnovations.com'),
    ('Bryan Hurst', 'bryan@newlinetechnicalinnovations.com'),
    ('Marshall Smith', 'marshall@newlinetechnicalinnovations.com'),
)

MANAGERS = ADMINS

if PRODUCTION and not socket.gethostname().startswith('nyx'):
    #TODOL mysql_pool
    DATABASES = {
        'default': {
            'ENGINE': 'django.db.backends.mysql', # Add 'postgresql_psycopg2', 'mysql', 'sqlite3' or 'oracle'.
            'NAME': 'DNASkittle', # Or path to database file if using sqlite3.
            'USER': 'skittle', # Not used with sqlite3.
            'PASSWORD': 'sk!77l3PandaDatabase%', # Not used with sqlite3.
            'HOST': '', # Set to empty string for localhost. Not used with sqlite3.
            'PORT': '', # Set to empty string for default. Not used with sqlite3.
        }
    }

else:
    DATABASES = {
        'default': {
            'ENGINE': 'django.db.backends.mysql', # Add 'postgresql_psycopg2', 'mysql', 'sqlite3' or 'oracle'.
            'NAME': 'SkittleTree', # Or path to database file if using sqlite3.
            'USER': 'skittle', # Not used with sqlite3.
            'PASSWORD': 'sk!77l3PandaDatabase%', # Not used with sqlite3.
            'HOST': '', # Set to empty string for localhost. Not used with sqlite3.
            'PORT': '', # Set to empty string for default. Not used with sqlite3.
        }
    }

DATABASE_WAIT_TIMEOUT = 28800

# Local time zone for this installation. Choices can be found here:
# http://en.wikipedia.org/wiki/List_of_tz_zones_by_name
# although not all choices may be available on all operating systems.
# In a Windows environment this must be set to your system time zone.
TIME_ZONE = 'America/Denver'

# Language code for this installation. All choices can be found here:
# http://www.i18nguy.com/unicode/language-identifiers.html
LANGUAGE_CODE = 'en-us'
os.environ['LANG'] = 'en_US.UTF-8'

SITE_ID = 1

# If you set this to False, Django will make some optimizations so as not
# to load the internationalization machinery.
USE_I18N = True

# If you set this to False, Django will not format dates, numbers and
# calendars according to the current locale.
USE_L10N = True

# If you set this to False, Django will not use timezone-aware datetimes.
USE_TZ = True

# Absolute filesystem path to the directory that will hold user-uploaded files.
# Example: "/home/media/media.lawrence.com/media/"
MEDIA_ROOT = SKITTLE_TREE_LOC + 'media/'

# URL that handles the media served from MEDIA_ROOT. Make sure to use a
# trailing slash.
# Examples: "http://media.lawrence.com/media/", "http://example.com/media/"
MEDIA_URL = SKITTLE_TREE_URL + 'media/'

# Absolute path to the directory static files should be collected to.
# Don't put anything in this directory yourself; store your static files
# in apps' "static/" subdirectories and in STATICFILES_DIRS.
# Example: "/home/media/media.lawrence.com/static/"
STATIC_ROOT = SKITTLE_TREE_LOC + 'static/'

# URL prefix for static files.
# Example: "http://media.lawrence.com/static/"
STATIC_URL = SKITTLE_TREE_URL + 'static/'

# Additional locations of static files
STATICFILES_DIRS = (
    #'/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/',
    # Put strings here, like "/home/html/static" or "C:/www/django/static".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
    SKITTLE_TREE_LOC + 'SkittleCore/UI/assets/',
)

# List of finder classes that know how to find static files in
# various locations.
STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
    #    'django.contrib.staticfiles.finders.DefaultStorageFinder',
)

# Make this unique, and don't share it with anybody.
SECRET_KEY = '!#@$t98ergv0h245@$%$Y$25fdsagqw4t897yqhrwedvg!#$!#$'

# List of callables that know how to import templates from various sources.
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.Loader',
    'django.template.loaders.app_directories.Loader',
    #     'django.template.loaders.eggs.Loader',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    # Uncomment the next line for simple clickjacking protection:
    # 'django.middleware.clickjacking.XFrameOptionsMiddleware',
)

ROOT_URLCONF = 'SkittleTree.urls'

# Python dotted path to the WSGI application used by Django's runserver.
WSGI_APPLICATION = 'SkittleTree.wsgi.application'

TEMPLATE_DIRS = (
    #'/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/',
    # Put strings here, like "/home/html/django_templates" or "C:/www/django/templates".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
    SKITTLE_TREE_LOC + 'SkittleCore/UI/',
    SKITTLE_TREE_LOC + 'DNAStorage/UI/',
    SKITTLE_TREE_LOC + 'SkittleTree/UI/',
)
TEMPLATE_CONTEXT_PROCESSORS = (
    "django.contrib.auth.context_processors.auth",
    "django.core.context_processors.debug",
    "django.core.context_processors.i18n",
    "django.core.context_processors.media",
    "django.core.context_processors.static",
    "django.core.context_processors.tz",
    "django.contrib.messages.context_processors.messages",
    "SkittleTree.context_processors.global_vars",
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django.contrib.humanize',
    'SkittleCore.Graphs',
    'SkittleCore',
    'DNAStorage',
    'Annotations',
    'Utilities',

    # Uncomment the next line to enable the admin:
    'django.contrib.admin',
    # Uncomment the next line to enable admin documentation:
    # 'django.contrib.admindocs',
)

if DEBUG:
    MIDDLEWARE_CLASSES += (
        'Utilities.debug.HotshotProfileMiddleware',
        'Utilities.debug.cProfileMiddleware',
    )

INTERNAL_IPS = ('127.0.0.1',)

# A sample logging configuration. The only tangible logging
# performed by this configuration is to send an email to
# the site admins on every HTTP 500 error when DEBUG=False.
# See http://docs.djangoproject.com/en/dev/topics/logging for
# more details on how to customize your logging configuration.
LOGGING = {
    'version': 1,
    'disable_existing_loggers': False,
    'filters': {
        'require_debug_false': {
            '()': 'django.utils.log.RequireDebugFalse'
        }
    },
    'handlers': {
        'mail_admins': {
            'level': 'ERROR',
            'filters': ['require_debug_false'],
            'class': 'django.utils.log.AdminEmailHandler'
        }
    },
    'loggers': {
        'django.request': {
            'handlers': ['mail_admins'],
            'level': 'ERROR',
            'propagate': True,
        },
    }
}
