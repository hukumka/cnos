#include <callAPI.h>
#include <api.h>

#define RETURNFROMCALL 	asm("push %0"::"r"(retAddr) );\
						asm("ret");

#define PREPARETOCALL	asm("mov %ebp, %esp\n");\
						asm("pop %ebp");\
						asm("pop %0":"=r"(retAddr));

uint32 retAddr;
static int printf(const char *format,...){
	PREPARETOCALL;
	asm("pushl %0"::"r"(PRINTF_ADDR));
	CALLAPI;
	RETURNFROMCALL;
	return 0; // NO WARNING. NO USE
}
