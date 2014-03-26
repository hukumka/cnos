#include <fat32.h>
#include <ata.h>

char buffer[512];

#define PARTITION_BASE 446
#define PARTITION_SIZE 16
#define PR_TYPE_OFFSET 4
#define PR_LBABEGIN_OFFSET 8

uint8 FS_Type[4];
uint8 FS_VolumeIdSector[4];

void LoadPartitions(){
	ATAPIO_Read(0,1,master,buffer);
	for(int i=0;i<4;++i){
		FS_Type[i] = buffer[ PARTITION_BASE + i*PARTITION_SIZE + PR_TYPE_OFFSET ];
		FS_VolumeIdSector[i] = *(uint32*)(buffer + PARTITION_BASE + i*PARTITION_SIZE + PR_LBABEGIN_OFFSET);
	}
}

/*
struct functions{
	FILE* (*fopen)(const char path);
} CurrentFunctions;
*/

uint32 SelectDisk( uint8 id ){
	switch( FS_Type[id] ){
		case 0xC:
			return Fat32_Init( FS_VolumeIdSector[id] );
		default:
			return 0;
	}
}
