#!/usr/bin/env bash

./build_files.sh

make run

##set -x
#
#
#qemu-system-i386 -fda ./build_os/os.bin # Simulate OS