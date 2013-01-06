#!/bin/bash

git pull
python ./manage.py collectstatic -v0 --noinput