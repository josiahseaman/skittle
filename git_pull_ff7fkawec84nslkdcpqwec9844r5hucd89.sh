#!/bin/bash
{
cd /var/www/skittle-production
git pull
python ./manage.py collectstatic -v0 --noinput
touch SkittleTree/wsgi.py
}