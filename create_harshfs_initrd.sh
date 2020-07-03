#!/usr/bin/env bash

pwd
cd initrd
gcc create_kernel_initrd.c -o create_kernel_initrd
./create_kernel_initrd
mv kernel_harshfs_initrd.bin ../build_os

