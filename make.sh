#!/bin/bash

[ -e ./build ] && rm -rf build

mkdir build build/boot build/kernel build/lib build/lib/kernel

set -xe

nasm -I ./boot/include -o ./build/boot/mbr.bin ./boot/mbr.S

dd if=./build/boot/mbr.bin of=./hd60M.img bs=512 count=1 conv=notrunc

nasm -I ./boot/include -o ./build/boot/loader.bin ./boot/loader.S

dd if=./build/boot/loader.bin of=./hd60M.img bs=512 count=4 seek=2 conv=notrunc

nasm -f elf32 -o ./build/lib/kernel/print.o ./lib/kernel/print.S

gcc -m32 -fno-pic -I./lib -c -o ./build/kernel/main.o kernel/main.c
# gcc -m32 -c -O1 -o ./build/kernel/main.o kernel/main.c

objcopy --remove-section .note.gnu.property ./build/kernel/main.o

ld -m elf_i386 -Ttext 0xc0001500 -e main -o ./build/kernel/kernel.bin ./build/kernel/main.o ./build/lib/kernel/print.o

dd if=./build/kernel/kernel.bin of=./hd60M.img bs=512 count=200 seek=9 conv=notrunc
