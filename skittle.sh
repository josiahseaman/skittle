#!/bin/bash

host=`hostname`
echo $host
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

delete_cache() {
    as_user "python $SKITTLE/manage.py deletecache $1"
}

case "$1" in
    wipeall)
        wipe_all_cache
        ;;
    deletecache)
            delete_cache "$var"
        if [ $# -ge 2 ];
        then
            list="$2"
            
            while shift && [ -n "$2" ]
            do
                list="${list},$2"
            done
            
            delete_cache $list
        else
            delete_cache $2
        fi
        ;;
    help|--help|-h)
        echo "This script will help you interact with the Skittle Server."
        echo "Normally you would need to be the www-data user to interact with the files properly, but this script will execute commands as that user for you."
        echo ""
        echo "To wipe the whole cache: wipeall"
        echo "To wipe the cache of a specific graph: deletecache # # # ..."
        echo ""
        ;;
    *)
        echo "No such command, see $0 help"
        exit 1
        ;;
esac

exit 0
