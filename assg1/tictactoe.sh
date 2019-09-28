#!/bin/bash

arr=("0" "0" "0" "0" "0" "0" "0" "0" "0")

game_run=true

while [ "$game_run" ]
do
    read -p "Enter your move" x y
    if [ $x -ge 3 ] || [ $y -ge 3 ]
    then
        echo INVALID MOVE
        continue

    fi
    move_num=$(((3*y)+x))
    free_moves=()
    for value in {0..8}
    do
        cur_ind=${arr[value]}
        if [ "$cur_ind" == "0" ]
        then
            free_moves+=($value)
        fi
    done

    move_allowed=false
    for value in ${free_moves[@]}
    do
        if [ "$value" == "$move_num" ]
        then
            move_allowed=true
        fi
    done

    echo $move_allowed
    if [ "$move_allowed" == true ]
    then
        arr[$move_num]="-1"
    else
        echo POSITION OCCUPIED
    fi

    free_moves=()

    
    for value in {0..8}
    do
        cur_ind=${arr[value]}
        if [ "$cur_ind" == "0" ]
        then
            free_moves+=($value)
        fi
    done


    if [ "${#free_moves[@]}" == 0 ]
    then
        echo NO EMPTY POSITION
        break
    fi

    

    selectedmove=${free_moves[$RANDOM % ${#free_moves[@]}]}
    arr[$selectedmove]="1"



done



