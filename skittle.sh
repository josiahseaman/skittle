#!/bin/bash

host=echo `hostname`

if [[ "$host" == *nyx* ]]
then
    SKITTLE="/var/www/skittle-development"
else
    SKITTLE="/var/www/skittle"
fi

ME=`whoami`
as_user() {
    if [ $ME == "www-data" ]
    then
        bash -c "$1"
    else
        sudo -u "www-data" -i /bin/bash -c "$1"
    fi
}

wipe_all_cache() {
    as_user "python $SKITTLE/manage.py deletecache all"
}

deletecache() {
    as_user "python $SKITTLE/manage.py deletecache $1"
}

case "$1" in
    wipeall)
        wipe_all_cache()
        ;;
    deletecache)
        wipe_graph($2)
        ;;
    help|--help|-h)
        echo "BLAH BLAH"
        ;;
    *)
        echo "No such command, see $0 help"
        exit 1
        ;;
esac

exit 0