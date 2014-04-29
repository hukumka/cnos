#pragma once
#include <types.h>

// PORTS
static inline uint8 inb(uint16 port){
	uint8 ret;
	asm volatile( "inb %1,%0" : "=a"(ret) : "Nd"(port) );
	return ret;
}
static inline uint16 inw(uint16 port){
	uint16 ret;
	asm volatile( "inw %1,%0" : "=a"(ret) : "Nd"(port) );
	return ret;
}
static inline uint32 inl(uint16 port){
	uint32 ret;
	asm volatile( "inl %1,%0" : "=a"(ret) : "Nd"(port) );
	return ret;
}
static inline void repInsw(uint16 port, uint16* data, uint16 count){
	asm volatile( "rep insw" :: "Nd"(port), "D"(data), "c"(count) );
}

static inline void outb(uint16 port, uint8 value){
	asm volatile( "outb %0, %1" : : "a"(value), "Nd"(port) );
}
static inline void outw(uint16 port, uint16 value){
	asm volatile( "outw %0, %1" : : "a"(value), "Nd"(port) );
}
static inline void outl(uint16 port, uint32 value){
	asm volatile( "outl %0, %1" : : "a"(value), "Nd"(port) );
}

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

static inline void io_wait(void){
	asm volatile ( 
		"jmp .+2\n"
		"jmp .+2\n"	
	);
}
// DATA

#define MOVE_ESDATA_TO_LOCAL(global,local)	asm("mov %%es:(%1), %0":"=r"(local):"r"(&global) )
#define MOVE_LOCAL_TO_ESDATA(local,global) 	asm("mov %0,%%es:(%1)"::"r"(local), "r"(&global) );
