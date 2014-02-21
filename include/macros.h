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

static inline void outb(uint16 port, uint8 value){
	asm volatile( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
static inline void outw(uint16 port, uint16 value){
	asm volatile( "outw %0, %1" : : "a"(val), "Nd"(port) );
}
static inline void outl(uint16 port, uint32 value){
	asm volatile( "outl %0, %1" : : "a"(val), "Nd"(port) );
}
