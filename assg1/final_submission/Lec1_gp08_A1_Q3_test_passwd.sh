#!/bin/bash

password=$1
if [ ${#password} -le 7 ]; then
	echo Length of password must be greater than equal to 8
	exit
fi

if [[ ! $password =~ [0-9] ]];then
      echo "Password does not contain number"
fi
if [[ $password != *['!'#\$%\&*+=-]* ]]
then
  echo "Password must have one of the specified special characters"
fi
