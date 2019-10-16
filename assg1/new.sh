#!/bin/bash


pass=$1
passCount=$(echo $1 | wc -c)
digitCount=$(echo $1 | grep -o "[0-9]" | wc -l)
pass2=$(echo $pass | egrep '[#$%&*+-=]+')
alphaNumericCount=$(echo $pass | grep "[#$%&*+-=]+")

echo $pass2
echo $alphaNumericCount
if [ $passCount -lt 8 ] || [ $digitCount -lt 1 ] ;
then
echo weak password
else
echo strong password
fi