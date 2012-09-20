#!/bin/bash
#NOTE: 'bc' must be installed for this script to work
# 'rar' and 'unrar' are also required
# 'lcab' is also required
# 'p7zip'...

EXT="*.fa"

#Find that there are actually some .fa files in this folder If not, then exit
COUNT=(`find ./ -maxdepth 1 -name "$EXT"`)
if [ ${#COUNT[@]} -le 0 ];
then
	echo -e "\n--------------------\nThere are no $EXT files to test against! Exiting...\n--------------------"
	exit
fi

# Go through all the .fa files in the folder and tally their size
TOTALPRE=0
for f in $EXT
do
	TEMP=$(stat -c%s $f)
	TOTALPRE=$(($TOTALPRE + $TEMP))
done
GOAL=$(($TOTALPRE / 4))

# See if a previous output file exists, if so then remove it
if [ -f compressionOutput.info ]
then
	rm compressionOutput.info
fi

echo -e "\n--------------------"
echo -e "Total size of all $EXT files in directory: $TOTALPRE \nGoal: $GOAL \nAttempting different compression tools on all $EXT files in the directory..." | tee -a compressionOutput.info

#--------------------------------------------------
# Begin gzip compression
START=$(date +%s)
echo -e "\nStarting gzip compression at: $START" | tee -a compressionOutput.info
tar -zcvf compressed.tgz $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a gzip format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.tgz)
if [ "$GOAL" \> "$TOTALPOST" ];
then
	PREFIX="-"
	POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
	PREFIX="+"
	POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The gzip compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin bzip2 compression
START=$(date +%s)
echo -e "\nStarting bzip2 compression at: $START" | tee -a compressionOutput.info
tar -jcvf compressed.tbz2 $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a bzip2 format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.tbz2)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The bzip2 compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin zip compression
START=$(date +%s)
echo -e "\nStarting zip compression at: $START" | tee -a compressionOutput.info
zip -9 compressed.zip $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a zip format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.zip)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The zip compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin rar solid compression
START=$(date +%s)
echo -e "\nStarting solid rar compression at: $START" | tee -a compressionOutput.info
rar a -m5 -s compressed.rar $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a solid rar format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.rar)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The solid rar compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin rar solid disabled compression
START=$(date +%s)
echo -e "\nStarting unsolid rar compression at: $START" | tee -a compressionOutput.info
rar a -m5 -s- compressedUnsolid.rar $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into an unsolid rar format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressedUnsolid.rar)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The unsolid rar compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin cab compression
START=$(date +%s)
echo -e "\nStarting cab compression at: $START" | tee -a compressionOutput.info
lcab $EXT compressed.cab
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into an cab format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.cab)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The cab compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin 7zip compression
START=$(date +%s)
echo -e "\nStarting 7zip compression at: $START" | tee -a compressionOutput.info
echo -e "Moving files to subfolder for this bit of execution."
mkdir temp
mv $EXT ./temp/
p7zip ./temp
echo -e "Moving files back to main folder after execution."
mv ./temp/* ./
rm -R ./temp
mv temp.7z compressed.7z
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a 7zip format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.7z)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The 7zip compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

#--------------------------------------------------
# Begin nanozip compression
START=$(date +%s)
echo -e "\nStarting nanozip compression at: $START" | tee -a compressionOutput.info
./nanozip/nz a -cc compressed.nz $EXT
END=$(date +%s)
DIFF=$(( $END - $START ))
echo -e "It took $DIFF seconds to compress all the $EXT files into a nanozip format." | tee -a compressionOutput.info
TOTALPOST=0
TOTALPOST=$(stat -c%s compressed.nz)
if [ "$GOAL" \> "$TOTALPOST" ];
then
        PREFIX="-"
        POSTDIFFERENCE=$(($GOAL - $TOTALPOST))
else
        PREFIX="+"
        POSTDIFFERENCE=$(($TOTALPOST - $GOAL))
fi
MATH="$TOTALPOST*100/$GOAL"
POSTPERCENT=$(echo "scale=4;${MATH}" | bc)
echo -e "The nanozip compressed size of all the $EXT files is: $TOTALPOST \n\tSize Difference: $PREFIX $POSTDIFFERENCE \n\tCompression Percent: $POSTPERCENT" | tee -a compressionOutput.info

echo -e "--------------------"
