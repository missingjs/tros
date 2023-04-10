#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/..)

hdfile=$proj_dir/hd60M.img

[ -e $hdfile ] || { echo "$hdfile not found"; exit 1; }

cd $self_dir

set -e

crt/build.sh

src/build-all.sh

python3 pack.py

size_file="size.bin"
size_seek=300

index_file="files.index"
index_sec_cnt=$(ls -l $index_file | awk '{printf("%d", ($5+511)/512)}')
index_seek=301

pack_file="package.bin"
pack_sec_cnt=$(ls -l $pack_file | awk '{printf("%d", ($5+511)/512)}')
pack_seek=400

set -x
dd if=$size_file of=$hdfile bs=512 count=1 seek=$size_seek conv=notrunc
dd if=$index_file of=$hdfile bs=512 count=$index_sec_cnt seek=$index_seek conv=notrunc
dd if=$pack_file of=$hdfile bs=512 count=$pack_sec_cnt seek=$pack_seek conv=notrunc

