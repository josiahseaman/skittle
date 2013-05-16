#!/bin/bash/

{
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

python ./manage.py collectstatic -v0 --noinput

if [ $# -gt 0 ]
then
    if [ $1 == "clean" ]
    then
        python ./manage.py deletecache all
    fi
fi

python ./manage.py purgeprocessqueue
python ./manage.py deletecache all
}