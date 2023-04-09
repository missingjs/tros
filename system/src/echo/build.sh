#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/../../..)
rootfs_dir=$proj_dir/system/rootfs

bin_name=echo
bin_path=/

target_dir=${rootfs_dir}${bin_path}
[ -e $target_dir ] || mkdir -p $target_dir

CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers"

set -xe

gcc $CFLAGS -I$proj_dir/include -I$proj_dir/include/lib -o main.o main.c

ld -m elf_i386 main.o $proj_dir/system/crt/tros_crt.a -o "echo"

mv $bin_name $target_dir
