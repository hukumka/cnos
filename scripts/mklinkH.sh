#!/bin/bash

function_name=printf
printf_addr=0x$(objdump --all-headers kernel.bin | grep '\<'$function_name'$' | awk '{print $1}' )
echo $printf_addr

echo "#pragma once">bin/cnos.h
echo "int (*printf)(const char *format,...)=(int(*)(const char*,...)) $printf_addr;">>bin/cnos.h
