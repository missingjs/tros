#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/../..)
rootfs_dir=$proj_dir/system/rootfs
CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers"

function build_exec()
{
    local setup_path=$1
    local bin_name=$(basename $setup_path)
    local bin_path=$(dirname $setup_path)
    local target_dir=${rootfs_dir}${bin_path}
    [ -e $target_dir ] || mkdir -p $target_dir

    set -x
    gcc $CFLAGS -I$proj_dir/include -I$proj_dir/include/lib *.c || exit
    ld -m elf_i386 -o $bin_name *.o $proj_dir/system/crt/mini-crt.a || exit
    mv $bin_name $target_dir
    set +x
}

cd $self_dir

for d in $(ls); do
    [ -d $d ] || continue
    cd $d
    [ -e exec.rule ] && build_exec $(cat exec.rule)
    cd ..
done
