#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)

cd $self_dir

for d in $(ls); do
    [ -d $d ] || continue
    cd $d
    [ -x ./build.sh ] && ./build.sh
    cd ..
done


