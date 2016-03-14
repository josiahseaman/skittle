# Django settings for SkittleTree project.
import os
import sys

PRODUCTION = True
OVERRIDE_DEBUG = False
if PRODUCTION:
    PRODUCTION_SETTINGS = True
    from SkittleTree.production_settings import *
else:
    PRODUCTION_SETTINGS = False
    from SkittleTree.development_settings import *
if OVERRIDE_DEBUG:
    DEBUG = True

    EMAIL_BACKEND = 'django.core.mail.backends.console.EmailBackend'

    os.environ['HTTPS'] = 'off'
    os.environ['wsgi.url_scheme'] = 'http'
    SESSION_COOKIE_SECURE = False
    CSRF_COOKIE_SECURE = False

# --------------------

CHUNK_SIZE = 65536

# --------------------

# Look for any settings to import from the installer
if os.path.isfile(os.path.join(BASE_DIR, 'settings.ini')):
    from importlib import machinery
    install_settings = machinery.SourceFileLoader('install_settings', os.path.join(BASE_DIR, 'settings.ini')).load_module()
    WORKSPACE_PATH = install_settings.WORKSPACE_PATH
if not WORKSPACE_PATH:
    if os.name == "nt":  # Windows users could be on a domain with a documents folder not in their home directory.
        try:
            import ctypes.wintypes
            CSIDL_PERSONAL = 5
            SHGFP_TYPE_CURRENT = 0
            buf = ctypes.create_unicode_buffer(ctypes.wintypes.MAX_PATH)
            ctypes.windll.shell32.SHGetFolderPathW(0, CSIDL_PERSONAL, 0, SHGFP_TYPE_CURRENT, buf)
            WORKSPACE_PATH = os.path.join(buf.value, "Skittle Workspace")
        except:
            WORKSPACE_PATH = None
    if not WORKSPACE_PATH:
        WORKSPACE_PATH = os.path.join(os.path.expanduser("~"), "Documents", "Skittle Workspace")
if not DB_BASE_DIR:
    DB_BASE_DIR = os.path.join(WORKSPACE_PATH, "db")
if not os.path.exists(WORKSPACE_PATH):
    os.makedirs(WORKSPACE_PATH)
if not os.path.exists(DB_BASE_DIR):
    os.makedirs(DB_BASE_DIR)

if sys.platform == 'win32':
    OS_DIR = 'windows'
    EXTENSION = '.exe'
    SCRIPT = '.cmd'
else:
    OS_DIR = 'linux'
    EXTENSION = ''
    SCRIPT = ''

SITE_ID = 1

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

WSGI_APPLICATION = 'SkittleTree.wsgi.application'
ROOT_URLCONF = 'SkittleTree.urls'

AUTH_USER_MODEL = 'SkittleCore.SkittleUser'
#LOGIN_REDIRECT_URL = '/'

LANGUAGE_CODE = 'en-us'
os.environ['LANG'] = 'en_US.UTF-8'
TIME_ZONE = 'UTC'
USE_I18N = True
USE_L10N = True
USE_TZ = True
LOCALE_PATHS = []
if getattr(sys, 'frozen', False):
    for app_name in INSTALLED_APPS:
        if os.path.exists(os.path.join(BASE_DIR, 'locale', app_name)):
            LOCALE_PATHS += (os.path.join(BASE_DIR, 'locale', app_name), )

STATIC_ROOT = os.path.join(BASE_DIR, 'static')
STATIC_URL = '/static/'
STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
    #    'django.contrib.staticfiles.finders.DefaultStorageFinder',
)
STATICFILES_DIRS = (
    #'/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/',
    # Put strings here, like "/home/html/static" or "C:/www/django/static".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
    os.path.join(BASE_DIR, 'SkittleTree', 'UI', 'assets'),
    os.path.join(BASE_DIR, 'SkittleCore', 'UI', 'assets'),
    os.path.join(BASE_DIR, 'DNAStorage', 'UI', 'assets'),
)

WEBPACK_LOADER = {
    'DEFAULT': {
        'BUNDLE_DIR_NAME': 'bundles/',
        'STATS_FILE': os.path.join(BASE_DIR, 'webpack-stats.json'),
    }
}

MEDIA_ROOT = os.path.join(BASE_DIR, 'media')
MEDIA_URL = '/media/'

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    # Uncomment the next line for simple clickjacking protection:
    # 'django.middleware.clickjacking.XFrameOptionsMiddleware',
)

TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.Loader',
    'django.template.loaders.app_directories.Loader',
    #     'django.template.loaders.eggs.Loader',
)
TEMPLATE_DIRS = (
    #'/Users/marshallds/Sites/Skittle/master/SkittleCore/UI/',
    # Put strings here, like "/home/html/django_templates" or "C:/www/django/templates".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
    os.path.join(BASE_DIR, 'SkittleCore', 'UI'),
    os.path.join(BASE_DIR, 'DNAStorage', 'UI'),
    os.path.join(BASE_DIR, 'SkittleTree', 'UI'),
)
TEMPLATE_CONTEXT_PROCESSORS = (
    "django.contrib.auth.context_processors.auth",
    "django.core.context_processors.debug",
    "django.core.context_processors.i18n",
    "django.core.context_processors.media",
    "django.core.context_processors.static",
    "django.core.context_processors.tz",
    "django.contrib.messages.context_processors.messages",
    "django.core.context_processors.request",
    "SkittleTree.context_processors.global_vars",
)

if DEBUG:
    MIDDLEWARE_CLASSES += (
        'Utilities.debug.HotshotProfileMiddleware',
        'Utilities.debug.cProfileMiddleware',
    )

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
