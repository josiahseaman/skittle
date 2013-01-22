#!/bin/bash/

{
python ./manage.py collectstatic -v0 --noinput
python ./manage.py evolve --hint --execute --noinput
python ./manage.py runserver 5000
}