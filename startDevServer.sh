#!/bin/bash/

{
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

./cleanup.sh

python ./manage.py runserver 5000
}