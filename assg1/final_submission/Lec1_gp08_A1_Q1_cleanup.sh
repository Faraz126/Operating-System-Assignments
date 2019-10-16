#!/bin/bash


num=$#

if [ $num == 2 ]
then
    path=$1
    directory=$2
    totaloFiles=0
    totaltxtFiles=0
    if [ -s "$path" ] && [ -d "$directory" ]
    then
        number=$(cut -f 2 -d ',' "$path" | wc -l)
        echo Total Student records: $number
        
        while IFS=, read -r col1 col2 col3 col4
        do
            echo $totaloFiles
            echo "I got:$col2"
            echo $directory/st$col2
            if [ -d "$directory"/st"$col2" ]
            then
                numoFiles=$(find $directory/st$col2 -type f -name '*.o' | wc -l)
                find $directory/st$col2 -type f -name '*.o' -exec rm -rf {} \;
                echo Files deleted 'in' $directory/st$col2: $numoFiles
                totaloFiles=$((totaloFiles + numoFiles))
                txtFiles=$(find $directory/st$col2 -type f -name '*.txt')
                numtextFiles=0
                for file in $txtFiles;
                do
                    firstLine=$(head $file)
                    if [ "$firstLine" == "#/bin/bash" ]
                    then
                        mv "$file" "${file%.*}.sh"
                        numtextFiles=$((numtextFiles + 1))
                    fi
                done
                echo Files renamed 'in' $directory/st$col2: $numtextFiles 
                totaltxtFiles=$((totaltxtFiles + numtextFiles))
            else
                echo ERROR: $col3\'s directory was not found
            fi
        done < $path
    else
        echo ERROR: Either path or directory does not exist.
    fi
    echo TOTAL FILES DELETED: $totaloFiles
    echo TOTAL FILES RENAMED: $totaltxtFiles
else
    echo ERROR: "2 arguments not provided"
fi



