#!/bin/bash

function_name=callAPI
callAPI_addr=0x$(objdump --all-headers ../kernel.bin | grep '\<'$function_name'$' | awk '{print $1}' )
echo $printf_addr

echo "#define CODE_SELECTOR 8">include/callAPI.h
echo "int (*callAPI)(uint16 id)=(int(*)(uint16)) $callAPI_addr;">>include/callAPI.h
