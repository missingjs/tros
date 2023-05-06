#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/../..)

cd $self_dir

INCLUDES="-I. -I$proj_dir/include -I$proj_dir/include/lib"
CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers $INCLUDES"
CRT_LIB=mini-crt.a

rm $CRT_LIB *.o 2>/dev/null

set -e

for sf in $(ls *.c); do
    echo "gcc $CFLAGS $sf"
    gcc $CFLAGS $sf
done

set -x

nasm -f elf32 ./start.S -o ./start.o

ar rcs $CRT_LIB *.o
