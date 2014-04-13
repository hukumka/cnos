#include <types.h>
#include <io.h>
#include <ata.h>
#include <memory.h>
#include <pci.h>
#include <fat32.h>
#include <fs.h>

int main(){
	InitMemoryTable();
	LoadPartitions();
	SelectDisk(0);
	ClearScreen();
	FAT32_FILE f;
	Fat32_open( 2, "bin/0.bin", &f);
	void *data = Allocate( f.fileSize/512 + 1 );
	Fat32_Read(&f,data,f.fileSize);
	void (*prog)() = data;
	prog();
	return 0;
}
