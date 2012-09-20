#!/bin/bash

EXT="*.fa"

shopt -s nullglob

for f in $EXT
do
	tr '[:lower:]' '[:upper:]' < $f > temp.txt
	rm $f
	mv temp.txt $f

	sed -i 's/N//g' $f

#	sed -i 's/\n//g' $f
#	sed -i 's/$0//g' $f
#	sed -i 's/\0//g' $f
#	sed -i 's/\r//g' $f
	perl -e 'while (<>) { chomp; print; }; exit;' $f > temp.txt
	rm $f
	mv temp.txt $f
	echo $f
#	awk '{ printf "%s", $0 }' $f
done
