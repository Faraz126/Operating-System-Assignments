#!/bin/bash

pass=$1
pass2=$(echo $pass | egrep -o "['$'#%&*+-=]+")
pass3=$(echo $pass | egrep -o "[0-9]+")

echo $pass2
if [ ${#pass} -ge 8 ] && [ -n "$pass2" ] && [ $pass3  ]
then
    echo VALID PASSWWORD
else
    echo UNVALID PASSWWORD
fi