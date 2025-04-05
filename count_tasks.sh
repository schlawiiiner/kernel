#!/bin/bash

COUNTER=0

for source_file in "$@"
do 
    object_file="${source_file/src/bin}"
    object_file="${object_file/.c/.o}"
    object_file="${object_file/.asm/.o}"
    if [ ! -f $object_file ]; then
        COUNTER=$(( COUNTER + 1 ))
        continue
    fi

    source_last_modified=`stat -c "%Y" $source_file`
    object_last_modified=`stat -c "%Y" $object_file`

    if [ "$source_last_modified" -gt "$object_last_modified" ]; then 
        COUNTER=$(( COUNTER + 1 ))
    fi
done

# one extra task for linking
COUNTER=$(( COUNTER + 1 ))
echo $COUNTER

