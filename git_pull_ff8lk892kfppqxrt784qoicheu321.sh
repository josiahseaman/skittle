#!/bin/bash
.
{
cd /var/www/skittle
git pull
python ./manage.py collectstatic -v0 --noinput
}