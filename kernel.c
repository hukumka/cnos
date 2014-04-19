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

int main(){
	InitMemory();
	InitFilesystemManager();
	SelectDisk(0);
	ClearScreen();

	InitNewGDT(0x8000);

	CreateProcess( "bin/0.bin" );
	return 0;
}
