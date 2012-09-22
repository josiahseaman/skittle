#!/bin/bash
#requires md5sum

TARGET="*.*" # relative path to target files

touch files.list #create file in case it doesn't exist
cat /dev/null > files.list #clear file in case you've already done this before

for file in $TARGET
do
	if [ $file != "files.list" ] && [ $file != "get-md5s.sh" ]; then
		md5sum "$file" >> files.list
	fi
done