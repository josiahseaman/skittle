When starting the dev server, you can pass in the argument "clean" (./startDevServer.sh clean) to clear the cache for ALL graphs.

To import all .fa and .fasta files in the "DNAStorage/to_import" folder, run "manage.py importfasta"

Management Commands
===================

[![Join the chat at https://gitter.im/josiahseaman/skittle](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/josiahseaman/skittle?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

```shell
python manage.py blah

deletecache [graph_type graph_type ...] -e|--specimen=[specimen] -c|--chromosome=[chromosome] -s|--start=[start] -> Delete the cache of a certain graph type (Example: `python manage.py deletecache h --specimen=hg18 --chromosome=chrY-sample --start=65537`

purgeprocessqueue -> Purges all processes in the process queue database

getroundedindex [start_value] -> Tells you the actual chunk starting value for any given start index.

listgraphs -> List all registered graphs on the server (just a list of symbols)

importfasta -> Imports all the .fa or .fasta files that are correctly named in the DNAStorage/to_import folder

createSnpIndex -> Creates the SNP index database for reading 23andMe user info (Only needs to run once in server life)
```


Graph Types
===========

+ a : Annotation Display
+ b : Nucleotide Bias
+ h : Sequence Highlighter (Raster)
+ m : Repeat Map
+ o : Oligomer Usage
+ n : Nucleotide Display (Raster)
+ s : Similarity Heatmap
+ t : Threemer Detector


Apache Compile Settings
=======================

```shell
apt-get install apache2 apache2.2-common apache2-doc apache2-mpm-prefork apache2-utils libexpat1 ssl-cert libapache2-mod-php5 php5 php5-common php5-gd php5-mysql php5-imap php5-cli php5-cgi libapache2-mod-fcgid apache2-suexec php-pear php-auth php5-mcrypt mcrypt php5-imagick imagemagick libapache2-mod-suphp

apt-get install apache2-mpm-worker libapache2-mod-fcgid

./buildconf
./configure --prefix=/etc/apache2 --enable-mods-shared=all --enable-http --enable-deflate --enable-expires --enable-slotmem-shm --enable-headers --enable-rewrite --enable-proxy --enable-proxy-balancer --enable-proxy-http --enable-proxy-fcgi --enable-mime-magic --enable-log-debug --with-mpm=worker --enable-modules=all --with-included-apr --with-expat=builtin --enable-ssl --enable-proxy-connect --enable-cache --enable-disk-cache --enable-mem-cache --enable-nonportable-atomics=yes
make && make install

sudo make-ssl-cert generate-default-snakeoil --force-overwrite
```

Don't forget to edit the /etc/logrotate.d/apache2 file so it saves logs as 665 root www-data
Also check file chain /var/log/apache2 to make sure that www-data can get in a rw

PHP Compile Settings
====================

```shell
./configure --enable-bcmath --enable-calendar --enable-dba --enable-exif --enable-ftp --enable-mbstring --enable-shmop --enable-sigchild --enable-soap --enable-sockets --enable-sqlite-utf8 --enable-sysvmsg --enable-wddx --enable-zip --with-apxs2=/etc/apache2/bin/apxs --with-bz2 --with-config-file-path=/etc/apache2/conf --with-curl --with-gd --with-gettext --with-mcrypt --with-mysql --with-mysqli --with-openssl --with-pdo-mysql --with-pdo-pgsql --with-pgsql --with-xmlrpc --with-zlib
make && make test && make install
cp ./php-5.4.x/php.ini-development /etc/apache2/conf/php.ini
vi /etc/apache2/conf/extra/httpd-php5.conf
```

Add to httpd.conf

MOD_WSGI Compile Settings
=========================

```shell
wget http://modwsgi.googlecode.com/files/mod_wsgi-3.4.tar.gz
tar zxvf mod_wsgi-3.4.tar.gz
make clean
make distclean
./configure --with-apxs=/etc/apache2/bin/apxs
make
make install
```

Other Installs
==============

```shell
pip install SQLAlchemy
http://docs.sqlalchemy.org/en/rel_0_8/intro.html#installation
mysql "show GLOBAL variables;" "set global wait_timeout = 28800;"
```

Put mysql_pool into /usr/lib/python2.7
http://www.lfd.uci.edu/~gohlke/pythonlibs/#mysql-python


Thread Configurations
=====================
Apache httpd config under worker module:
StartServers 2
ServerLimit 16
MaxClients 400
MinSpareThreads 25
MaxSpareThreads 75
ThreadsPerChild 25

WSGIDaemonProcess
processes=8
threads=2
Note, this is static and doesn't create more as requests come in. The two times each other is the total number of python interpreters spawned and the max number of python requests that can be handled.
Static files and assets are NOT served off of this. They are served off of the Apache threads that are dynamically spawned from above.

Compile C Functions
===================
1) Create your .c file
2) Convert it to an object file `gcc -O3 -c -fPIC yourFile.c -o yourFile.o`
3) Create Shared Library from object file `gcc -shared -Wl,-soname,libYourLib.so.1 -o libYourLib.so.1.0.1 yourFile.o`
4) import ctypes
5) yourLib = ctypes.CDLL('/path/to/libYourLib.so.1.0.1')
6) yourLib.MyFunction()

On Windows
1) Open Visual Studio x64 Win64 Command Prompt
2) cd into SkittleCore\Graphs
3) Run `cl /Ox /LD SkittleGraphUtils.c /FelibSkittleGraphUtils`  # NOTE: Yes, there is no space after /fe and the file name.

If the function returns a string do this) returnedString = ctypes.c_char_p(yourLib.MyFunction()); print returnedString.value
NOTE: Python C Data Types: http://docs.python.org/2/library/ctypes.html#fundamental-data-types

To convert a list to C array) arr = (ctypes.c_int * len(yourlist))(*yourlist)

Database Evolutions
===================   
ANY changes to a model file must be passed to Bryan for a custom evolution script to be created.

Notes to self
=============
This is for dealing with multi-thread access to the database.
We need a persistent connection, but django doesn't support this until the future 1.6 release.
For a temporary fix, edit the django source:
django/db/__init__.py comment out signals.request_finished.connect(close_connection)
django/middleware/transaction.py remove the two transaction.is_dirty() and call commit() instead.
