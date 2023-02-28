#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)

cd $self_dir

source ./env.sh || exit

primary_hd=hd60M.img

[ -e $primary_hd ] && { echo "$primary_hd already exists"; exit; }

set -xe

bximage -hd -mode="flat" -size=60 -q hd60M.img
