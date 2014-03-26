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
	FAT32_FILE f;
	Fat32_open( 2, "boot/grub/menu.lst", &f);
	char data[513];
	int size = Fat32_Read( &f, data, 512 );
	data[size]=0;
	return 0;
}
