#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

for file in "$DIR/"*.in; do
    echo $file
    cat $file | "$DIR/../bin/iss"
done
