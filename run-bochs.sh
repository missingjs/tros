#!/bin/bash

self_dir=$(cd $(dirname $0) && pwd)

cd $self_dir

source ./env.sh || exit

set -xe

bochs -f bochsrc.disk
