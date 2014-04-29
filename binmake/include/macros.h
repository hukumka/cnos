#pragma once
#include <types.h>

// PM
static inline void longcall(uint16 cs, uint32 eip){
	struct __attribute__ (( __packed__ )){
		uint8 command;
		uint32 neweip;
		uint16 newcs;
		uint8 ret;
	} longcall_data;
	longcall_data.command = 0x9a;
	longcall_data.neweip = eip;
	longcall_data.newcs = cs;
	longcall_data.ret=0xc3;
	asm( "call *%0" :: "r"(&longcall_data) );
}

static inline void longjump(uint16 cs, uint32 eip){
	struct __attribute__ (( __packed__ )){
		uint8 command;
		uint32 neweip;
		uint16 newcs;
	} longcall_data;
	longcall_data.command = 0xea;
	longcall_data.neweip = eip;
	longcall_data.newcs = cs;
	asm( "jmp *%0" :: "r"(&longcall_data) );
}

#define _exit(rez) asm volatile("mov %0,%%eax\n"\
								"mov %%ebp, %%esp\n" \
								"pop %%ebp\n" \
								"lret\n" :: "r"((uint32)(rez)) )
