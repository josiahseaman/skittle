#!/bin/bash
{
cd /var/www/skittle
git pull
python ./manage.py collectstatic -v0 --noinput
python ./manage.py evolve --hint --execute --noinput
touch SkittleTree/wsgi.py
}