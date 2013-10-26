#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SUCCESS=0
FAIL=0

for file in "$DIR/"*.in; do
    echo $file
    cat $file | timeout 11 "$DIR/../bin/iss"

    if [ $? -eq 0 ]; then 
        $((SUCCESS += 1))
    else
        echo " --- Time Limit Exceeded --- "
        $((FAIL += 1))
    fi

    echo Successful maps: $SUCCESS Failed maps: $FAIL
done
