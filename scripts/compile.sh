#!/bin/bash

echo $#
if [[ $# < 1 ]]; then
	exit
fi
CFLAGS="-Wall -fno-builtin -nostdinc -nostdlib -std=gnu99 -m32 -I./include"

echo "Компиляция файла "$1" в "${1%.*}
gcc ${CFLAGS} -c $1 -o ${1%.*}.o
objcopy -O binary ${1%.*}.o ${1%.*}
rm ${1%.*}.o
