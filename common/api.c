#include <io.h>
#include <fs.h>
#include <fat32.h>
#include <api.h>

uint32 retAddr; 
uint32 retSelector;

//						esp-2	  esp
// 0x0000 0x00107356 :=:0x00 0x00 0x56 0x73 0x10 0x00
void callAPI( uint32 funcAddr){
	// Убираем рамку стека
	asm volatile( 	"mov %ebp, %esp\n"
					"pop %ebp\n" );
	// Сохраняем адрес возврата в глобальных переменных
	asm volatile(	"pop %%eax\n"
					"mov %%eax, %%es:(%%ebx)":: "b"(&retAddr) );
	asm volatile(	"pop %%eax\n"
					"mov %%eax, %%es:(%%ebx)":: "b"(&retSelector) );
	
	asm volatile( 	"pop %eax\n"
					"call *%eax\n" );
	
	asm volatile(	"mov %%es:(%%ebx), %%eax\n"
					"push %%eax":: "b"(&retSelector) );
	asm volatile(	"mov %%es:(%%ebx), %%eax\n"
					"push %%eax":: "b"(&retAddr) );
	
	asm volatile(	"lret\n" );
}
