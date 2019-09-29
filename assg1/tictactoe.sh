#!/bin/bash

arr=("_" "_" "_" "_" "_" "_" "_" "_" "_")

game_run=true

while [ "$game_run" == true ]
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
        if [ "$cur_ind" == "_" ]
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
        arr[$move_num]="X"
    else
        echo POSITION OCCUPIED
        continue
    fi

    for i in 0 3 6
    do
        start_pos=${arr[$i]}
        pos_1=${arr[$((i+1))]}
        pos_2=${arr[$((i+2))]}
        sleep 0.1
        if [ "$start_pos" != "_" ] && [ "$pos_1" == "$start_pos" ] && [ "$pos_2" == "$start_pos" ]
        then
            echo GAME END
            game_run=false
            break
            sleep 2
        else
            echo game not end
        fi

    done

    for i in 0 1 2
    do
        start_pos=${arr[$i]}
        if [ "$start_pos" != "_" ] && [ "${arr[$((i+3))]}" == "$start_pos" ] && [ "${arr[$((i+6))]}" == "$start_pos" ]
        then
            echo GAME END
            game_run=false
            break
        else
            echo game not end
        fi
    done

    if [ "${arr[0]}" != "_" ] && [ "${arr[4]}" == "${arr[0]}" ] && [ "${arr[8]}" == "${arr[0]}" ] 
    then
        echo GAME END
        game_run=false
    fi

    if [ "${arr[2]}" != "_" ] && [ "${arr[4]}" == "${arr[2]}" ] && [ "${arr[6]}" == "${arr[2]}" ] 
    then
        echo GAME END
        game_run=false
    fi

    free_moves=()

    
    for value in {0..8}
    do
        cur_ind=${arr[value]}
        if [ "$cur_ind" == "_" ]
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
    arr[$selectedmove]="O"

    cols=$( tput cols )
    rows=$( tput lines )
    input_length=6
    half_input_length=$(( $input_length / 2 ))
    middle_row=$(( ($rows / 2) - 1 ))
    middle_col=$(( (($cols / 2) - $half_input_length)-1 ))

    tput clear
    tput bold

    tput cup $middle_row $middle_col

    echo ${arr[0]} ${arr[1]} ${arr[2]}

    middle_row=$(( ($rows / 2) ))
    middle_col=$(( (($cols / 2) - $half_input_length) - 1 ))

    tput cup $middle_row $middle_col

    echo ${arr[3]} ${arr[4]} ${arr[5]}

    middle_row=$(( ($rows / 2) +1 ))
    middle_col=$(( (($cols / 2) - $half_input_length) -1 ))

    tput cup $middle_row $middle_col

    echo ${arr[6]} ${arr[7]} ${arr[8]}
    tput bold
    
done



