#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/../..)

cd $self_dir

CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -I. -I$proj_dir/include -I$proj_dir/include/lib"

rm *.o 2>/dev/null

set -xe

for sf in $(ls *.c); do
    gcc $CFLAGS $sf
done

nasm -f elf32 ./start.S -o ./start.o

ar rcs mini-crt.a *.o
