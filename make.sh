#!/bin/bash

[ -d ./build ] || mkdir ./build

set -xe

nasm -I ./include -o ./build/mbr.bin ./boot/mbr.S

dd if=./build/mbr.bin of=./hd60M.img bs=512 count=1 conv=notrunc

nasm -I ./include -o ./build/loader.bin ./boot/loader.S

dd if=./build/loader.bin of=./hd60M.img bs=512 count=4 seek=2 conv=notrunc
