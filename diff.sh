#!/bin/bash

origin_dir=$1

[ -z $1 ] && { echo "usage: diff.sh <origin-dir>"; exit 1; }

# compare include
while read filename; do
    real_path=${filename#./include/}
    origin_path=$origin_dir/$real_path
    if [ ! -e $origin_path ]; then
        echo "$filename not found at origin directory: $origin_path"
        exit 2
    fi
    cat $filename | grep -v '#include' | grep -vP '^\s*$' | sed 's/\r//g' | sed 's/\s*$//g' | sed 's/^\s*//g' > a
    cat $origin_path | grep -v '#include' | grep -vP '^\s*$' | sed 's/\r//g' | sed 's/\s*$//g' | sed 's/^\s*//g' > b
    diff a b || { echo "********" $filename : $origin_path "********"; }
done < <(find ./include -name "*.h")

# compare src
while read filename; do
    real_path=${filename#./src/}
    origin_path=$origin_dir/$real_path
    if [ ! -e $origin_path ]; then
        echo "$filename not found at origin directory: $origin_path"
        exit 2
    fi
    [ $filename == './src/boot/mbr.S' ] && continue
    cat $filename | grep -v '[#%]include' | grep -vP '^\s*$' | sed 's/\r//g' | sed 's/\s*$//g' | sed 's/^\s*//g' > a
    cat $origin_path | grep -v '[#%]include' | grep -vP '^\s*$' | sed 's/\r//g' | sed 's/\s*$//g' | sed 's/^\s*//g' > b
    diff a b || { echo "********" $filename : $origin_path "********"; }
done < <(find ./src -name "*.c" -o -name "*.S")

rm a b
