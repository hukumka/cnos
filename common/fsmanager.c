#include <fsmanager.h>
#include <ata.h>
#include <memory.h>

#define PARTITION_TABLE 0x1be
#define PARTITION_SIZE 0x10
#define TYPE 0x4

#define FS_FAT32_LBA 0xC

typedef struct{
	uint8 id;
	
} partition;

bool SelectPartition(uint8 id){
	uint8* bootsector = (uint8*)Allocate(1);
	ATAPIO_Read( 0, 1, master, bootsector);
	switch( bootsector[PARTITION_TABLE + PARTITION_SIZE*id + TYPE] ){
		case FS_FAT32_LBA:
			
			return true;
		default:
			return false;
	}
}
