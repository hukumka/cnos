#!/bin/bash

echo $#
if [[ $# < 1 ]]; then
	exit
fi
CFLAGS="-Wall -fno-builtin -nostdinc -nostdlib -std=gnu99 -m32 -I./include"

echo "Компиляция файла "$1" в "${1%.*}
gcc $CFLAGS $1 -o ${1%.*}.s -S
ld ${1%.*}.o -o ${1%.*}.bin -Ttext 0x0 -e main -oformat binary -m elf_i386
