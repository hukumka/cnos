#!/bin/bash

echo $#
if [[ $# < 1 ]]; then
	exit
fi
CFLAGS="-Wall -fno-builtin -nostdinc -nostdlib -std=gnu99 -m32 -I./include"

echo "Компиляция файла "$1" в "${1%.*}
gcc $CFLAGS $1 -o ${1%.*}.o -c -include 'cnos.h'
ld ${1%.*}.o -o ${1%.*}.bin -T linker.ld -m elf_i386
