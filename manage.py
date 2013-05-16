#!/usr/bin/env python
import os
import sys
import socket

if socket.gethostname().startswith('nyx') or socket.gethostname().startswith('skittle'):
    os.setuid(33)
    import getpass
    print getpass.getuser()

if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")

    from django.core.management import execute_from_command_line

    execute_from_command_line(sys.argv)
