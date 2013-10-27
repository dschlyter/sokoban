#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SUCCESS=0
FAIL=0

for file in "$DIR/"*.in; do
    echo $file
    cat $file | timeout 11 "$DIR/../bin/iss"

    RET=$?
    if [ $RET -eq 0 ]; then 
        SUCCESS=$((SUCCESS + 1))
    else
        if [ $RET -eq 1 ]; then
            echo; echo; echo " !!!!! - Invalid solution - !!!!!"
            echo; echo
            echo "Press enter to continue"
            read
        else 
            echo " --- Time Limit Exceeded --- "
        fi
        FAIL=$((FAIL + 1))
    fi

    echo Successful maps: $SUCCESS Failed maps: $FAIL
done
