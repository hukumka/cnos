#!/bin/bash

function_name=callAPI
callAPI_addr=0x$(objdump --all-headers ../kernel.bin | grep '\<'$function_name'$' | awk '{print $1}' )

function_name=printf
printf_addr=0x$(objdump --all-headers ../kernel.bin | grep '\<'$function_name'$' | awk '{print $1}' )


echo "#define CALLAPI asm(\"lcall \$0x8, \$$callAPI_addr\")">include/callAPI.h
echo "#define PRINTF_ADDR $printf_addr">>include/callAPI.h
