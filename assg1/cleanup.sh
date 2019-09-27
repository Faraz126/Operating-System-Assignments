#!/bin/bash


num=$#

if [ $num == 2 ]
then
    path=$1
    directory=$2
    if [ -s "$path" ] && [ -d "$directory" ]
    then
        number=$(cut -f 2 -d ',' "$path" | wc -l)
        echo Total Student records: $number
        
        while IFS=, read -r col1 col2 col3 col4
        do
            echo "I got:$col2"
            echo $directory/st$col2
            if [ -d "$directory"/st"$col2" ]
            then
                numoFiles=$(find $directory/st$col2 -type f -name '*.o' | wc -l)
                find $directory/st$col2 -type f -name '*.o' -exec rm -rf {} 
                echo $numoFiles
            else
                echo ERROR: $col3\'s directory was not found
            fi
        done < $path
    else
        echo ERROR: Either path or directory does not exist.
    fi
    echo $num
    echo $path
    echo $directory
else
    echo ERROR: "2 arguments not provided"

fi



