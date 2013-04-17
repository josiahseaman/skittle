#!/bin/bash/

{
python ./manage.py collectstatic -v0 --noinput

if [ $# -gt 0 ]
then
    if [ $1 == "clean" ]
    then
        python ./manage.py deletecache all
    fi
fi

python ./manage.py purgeProcessQueue
python ./manage.py runserver 5000
}