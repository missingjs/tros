#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/../..)

cd $self_dir

# BIN="cat"
CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers"
OBJS="$proj_dir/build/lib/stdio.o $proj_dir/build/lib/string.o $proj_dir/build/lib/user/assert.o $proj_dir/build/lib/user/syscall.o"

set -xe

nasm -f elf32 ./start.S -o ./start.o
ar rcs tros_crt.a $OBJS ./start.o
