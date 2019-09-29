#!/bin/bash

pass=$1
pass2=$(echo $pass | egrep '[#$%&*+-=]+')
pass3=$(echo $pass | egrep "[0-9]+")


if [ ${#pass} -ge 8 ] && [ -n "${pass2}" ] && [ -n "${pass3}" ]
then
    echo VALID PASSWWORD
else
    echo UNVALID PASSWWORD
fi