#!/bin/bash

filename=$1
directory=$2
if [ $# != 2 ]
then
	echo The number of arguments is $# when 2 we are asked.
	exit
fi

if [ -f "$filename" ]
then
	if [ ! -s $filename ]
	then
     		echo "File empty"
	fi
	echo $filename exists
else
	echo $filename does not exist or is not a 'file' or is empty
fi
if [ -d "$directory" ]
then
	echo $directory exists
else
	echo $directory does not exist or is not a directory
fi
id=`cat $filename| cut -d ',' -f2`
echo $id
name=`cat $filename| cut -d ',' -f3`
echo number of entries is
yas=`cat $filename | xargs -l | wc -l`
echo $yas
for i in $id
do
	if [ ! -d $2/st$i ] 
	then
		echo $2/st$i directory was not found
	fi
done
temp=`find $directory -name '*.o' -or -name '*~'`
counterDelete=`find $directory -name '*.o' -or -name '*~'| wc -l`
counterRename=0
if [ -z "$temp" ]
then
echo No .o or "~" files founds
else
find $directory -name '*.o' -or -name '*~'| xargs rm
fi
temp2=`find $directory -name '*.txt'`
for i in $temp2
do
	firstLine=$(head $i)
	if [ "$firstLine" == "#/bin/bash" ]; then
		filename="${i%.*}"
		counterRename=$((counterRename))	
		mv $i $filename.sh
	fi
done
echo number of deleted is $counterDelete and that of renamed is $counterRename
