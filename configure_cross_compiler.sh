#!/usr/bin/env bash

if [ ! -d "build_os" ]; then
  mkdir build_os
fi
docker --version
docker images
docker pull sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest