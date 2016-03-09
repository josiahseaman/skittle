import os
import sys


if getattr(sys, 'frozen', False):
    BASE_DIR = os.path.dirname(sys.executable)
    MAIN_APP = os.path.basename(sys.executable)
else:
    BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    MAIN_APP = os.path.basename(os.path.dirname(os.path.abspath(__file__)))

WORKSPACE_PATH = os.path.join(BASE_DIR, 'Skittle Workspace')  # To detect automatically, leave set to None

DB_BASE_DIR = os.path.join(WORKSPACE_PATH, 'db')  # To have as 'settings' folder in WORKSPACE_PATH leave set to None

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = '84cdv@1tm6sv3lml(-u0kn1mp7vlcy=%ee$y4@=uup4&zqo3i)'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

# Database
# https://docs.djangoproject.com/en/1.7/ref/settings/#databases
DATABASES = {
    'default': {
        'NAME': os.path.join(DB_BASE_DIR, 'Skittle.sqlite3'),
        'ENGINE': 'django.db.backends.sqlite3',
        'TEST': {'NAME': os.path.join(DB_BASE_DIR, 'TestSkittle.sqlite3')},
    }
}
DATABASE_WAIT_TIMEOUT = 28800

EMAIL_BACKEND = 'django.core.mail.backends.console.EmailBackend'

DEFAULT_FROM_EMAIL = 'noreply@development.server'  # Users get emails from this address 'info@mydomain.com'
SERVER_EMAIL = 'server@development.server'  # Admins and Managers get emails from this address about errors 'server@mydomain.com'

os.environ['HTTPS'] = 'off'
os.environ['wsgi.url_scheme'] = 'http'
SESSION_COOKIE_SECURE = False
CSRF_COOKIE_SECURE = False