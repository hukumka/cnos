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
	struct Fat32_DirMember mem;
	Fat32_CreateFile( "boot/", "1234.qwe", false);

	Fat32_SearchMember( 2, "boot/1234.qwe", &mem);
	return 0;
}
