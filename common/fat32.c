#include <fat32.h>
#include <memory.h>
#include <ata.h>

#define PARTITION_BASE 0x1BE
#define PARTITION_SIZE 0x10

#define PR_TYPE_OFFSET 0x4
#define PR_LBABEGIN_OFFSET 0x8

#define FS_FAT32_LBA 0x0C

#define BOOTSECTOR FAT32VolumeId

#define FS_VOLUMEID_BYTE_PER_SECTOR_OFFSET 0xB
#define FS_VOLUMEID_SECTOR_PER_CLUSTER_OFFSET 0xD
#define FS_VOLUMEID_NUMBER_OF_REVERSED_SECTORS_OFFSET 0xE
#define FS_VOLUMEID_NUMBER_OF_FATS_OFFSET 0x10
#define FS_VOLUMEID_SECTORS_PER_FAT_OFFSET 0x24
#define FS_VOLUMEID_ROOT_DIRECTORY_FIRST_CLUSTER_OFFSET 0x2C
#define FS_VOLUMEID_SIGNATURE_OFFSET 0x1FE

uint32 FS_LBABeginSector[8];
uint8 FS_Type[8];
uint8 FAT32VolumeId[512];
uint8 FS_CURRENT_PARTITION;

void LoadPartitions(){
	ATAPIO_Read(0,1,master,FAT32VolumeId);
	for(int i=0;i<4;++i){
		FS_Type[i] = BOOTSECTOR[ PARTITION_BASE + i*PARTITION_SIZE + PR_TYPE_OFFSET ];
		FS_LBABeginSector[i] = *(uint32*)(BOOTSECTOR + PARTITION_BASE + i*PARTITION_SIZE + PR_LBABEGIN_OFFSET);
	}
}

uint8 CurrentFileSystem=0;
uint32 Fat32BeginLBA;
uint32 Fat32ClusterBeginLBA;

bool SelectPartition(uint8 id){
	FS_CURRENT_PARTITION=id;
	ATAPIO_Read( FS_LBABeginSector[id], 1, (drive)(id/4), FAT32VolumeId );
	if( *(uint16*)(FAT32VolumeId	+ FS_VOLUMEID_BYTE_PER_SECTOR_OFFSET) != 512 ){ // Проверяем размер сектора
		CurrentFileSystem=0;
		return false;
	}
	if( *(uint8*)(FAT32VolumeId + FS_VOLUMEID_NUMBER_OF_FATS_OFFSET) != 2 ){ // Провиряем количество FAT таблиц
		CurrentFileSystem=0;
		return false;
	}
	if( *(uint16*)(FAT32VolumeId + FS_VOLUMEID_SIGNATURE_OFFSET) != 0xAA55 ){ // Проверяем сигнатуру
		CurrentFileSystem=0;
		return false;
	}
	CurrentFileSystem=0x80 | id;
	return false;
}
