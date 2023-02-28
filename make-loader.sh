#!/bin/bash

set -xe

nasm -I include -o ./loader.bin loader.S

dd if=./loader.bin of=./hd60M.img bs=512 count=4 seek=2 conv=notrunc
