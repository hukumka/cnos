// HEADERS
#include <types.h>
#include <io.h>
#include <ata.h>
#include <memory.h>
#include <pci.h>
#include <fat32.h>
#include <fs.h>
#include <pm.h>
#include <run.h>
#include <pic.h>

int main(){
	InitMemory();
	InitFilesystemManager();
	SelectDisk(0);
	ClearScreen();
	printf("qwe\nqweqwe");
	InitNewGDT(0x8000);

	InitNewIDT(0x9000);
	/*CreateProcess( "bin/0.bin" );*/
	IRQ_set_mask( 0b11111101 );
	asm("sti\n");
	while(true){

	}
	return 0;
}
