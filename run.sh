#!/usr/bin/env bash

set -x
rm boot_sector.bin
nasm -f bin boot_sector_main.s -o boot_sector.bin
qemu-system-x86_64  boot_sector.bin