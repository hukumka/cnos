#include <io.h>
#include <fs.h>
#include <fat32.h>
#include <api.h>

uint32 retAddr; 
uint16 retSelector;
uint16 functionId;

void callAPI( uint16 function ){
	functionId = function;
	// Убираем рамку стека
	asm volatile( "mov %ebp, %esp\n" );
	asm volatile( "pop %ebp\n" );
	// Сохраняем адрес возврата в глобальных переменных
	asm volatile( "pop %0" : "=r"(retAddr) );
	asm volatile( "pop %0" : "=r"(retSelector) );
	switch( functionId ){
		case PRINTF_CODE:
			asm("call *%0"::"r"(&printf) );
			break;
		case CLEARSCREEN_CODE:
			asm("call *%0"::"r"(&ClearScreen) );
		default:
			break;
	}
	asm volatile( "push %0" :: "r"(retAddr) );
	asm volatile( "push %0" :: "r"(retSelector) );
}
