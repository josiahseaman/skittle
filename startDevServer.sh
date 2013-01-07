#!/bin/bash/

{
python ./manage.py collectstatic -v0 --noinput
python ./manage.py runserver 5000
}