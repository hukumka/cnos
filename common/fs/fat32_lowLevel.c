#include <types.h>
#include <ata.h>
#include <fat32.h>
#include <memory.h>
#include "fat32_lowLevel.h"

// VOLUME ID смещения
#define BYTE_PER_SECTOR_OFFSET 0xB // Байтов на сектор в представлении fatовского volumeId, должно быть 512
#define NUMBER_OF_FATS 0x10
#define SIGNATURE_OFFSET 0x1FE
#define NUMBER_OF_REVERSED_SECTORS_OFFSET 0xE
#define SECTORS_PER_FAT_OFFSET 0x24
#define ROOT_DIRECTORY_FIRST_CLUSTER_OFFSET 0x2C
#define SECTOR_PER_CLUSTER_OFFSET 0xD

uint8 buffer0[ SECTOR_SIZE ];
uint32* FAT[2]={0,0};
uint32 SectorsPerFat_Old=0;
uint8 *ClusterData;
uint32 ClusterSize_old=0;

struct fat32_currparams CurrentParams;

bool Fat32_LoadFat(uint8 id){
	if(id>=2)
		return false;
	if( FAT[id] )
		Free( FAT[id], SectorsPerFat_Old);
	FAT[id] = (uint32*)Allocate( CurrentParams.SectorsPerFat );
	SectorsPerFat_Old = CurrentParams.SectorsPerFat;
	if( ATAPIO_Read( CurrentParams.FatBegin + id*CurrentParams.SectorsPerFat, CurrentParams.SectorsPerFat, master, FAT[id] ) == ATAPIO_DONE )
		return true;
	return false;
}
bool Fat32_SaveFat(uint8 id){
	if(id>=2)
		return false;
	return ATAPIO_Write( CurrentParams.FatBegin + id*CurrentParams.SectorsPerFat, CurrentParams.SectorsPerFat, master, FAT[id] );
}

bool Fat32_Init( uint32 volumeId_sector ){
	ATAPIO_Read( volumeId_sector, 1, master, buffer0 );
	if( *(uint16*)( buffer0 + BYTE_PER_SECTOR_OFFSET) != 512 )
		return false;
	if( *(uint8*)( buffer0 + NUMBER_OF_FATS) != 2 )
		return false;
	if( *(uint16*)( buffer0 + SIGNATURE_OFFSET ) != 0xAA55 )
		return false;
	CurrentParams.SectorsPerFat = *(uint32*)( buffer0 + SECTORS_PER_FAT_OFFSET );
	CurrentParams.SectorsPerCluster = *(uint8*)( buffer0 + SECTOR_PER_CLUSTER_OFFSET );
	CurrentParams.FatBegin = volumeId_sector + *(uint16*)( buffer0 + NUMBER_OF_REVERSED_SECTORS_OFFSET);
	CurrentParams.ClustersBegin = CurrentParams.FatBegin + 2*CurrentParams.SectorsPerFat;
	CurrentParams.RootDirCluster = *(uint32*)( buffer0 + ROOT_DIRECTORY_FIRST_CLUSTER_OFFSET );
	
	if(!Fat32_LoadFat(0))
		return false;
	if( ClusterSize_old )
		Free( ClusterData, ClusterSize_old);
	ClusterData = Allocate( CurrentParams.SectorsPerCluster );
	ClusterSize_old = CurrentParams.SectorsPerCluster;

	Fat32_SelectDirectory( CurrentParams.RootDirCluster );
	return true;
}

uint32 NextCluster( uint32 cluster ){
	return FAT[0][cluster];
}
bool LoadCluster( uint32 cluster, void *data ){
	return ATAPIO_Read( CurrentParams.ClustersBegin + (cluster-2) * CurrentParams.SectorsPerCluster, CurrentParams.SectorsPerCluster, master, data);
}
bool SaveCluster( uint32 cluster, void *data ){
	return ATAPIO_Write( CurrentParams.ClustersBegin + (cluster-2) * CurrentParams.SectorsPerCluster, CurrentParams.SectorsPerCluster, master, data);
}
