#!/usr/bin/env python
from django.db          import models
import os
import inspect

def currentPath():
    print os.getcwd()
    print inspect.stack()
