#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo "Usage: $0 <map>"
    exit 0
fi
g++ -g -O2 -pg *.cpp -std=gnu++0x
./a.out < $1
gprof | less -S
rm a.out
rm gmon.out
