#include <fat32.h>

bool Fat32_LoadFat(uint8);
bool Fat32_SaveFat(uint8);

bool Fat32_Init( uint32 );

bool Fat32_LoadCluster( uint32, void* );
bool Fat32_SaveCluster( uint32, void* );
uint32 Fat32_NextCluster( uint32 );

struct fat32_currparams{
	uint32 SectorsPerFat;
	uint8 SectorsPerCluster;
	uint32 RootDirCluster;
	uint32 FatBegin;
	uint32 ClustersBegin;
};

extern struct fat32_currparams CurrentParams;
extern uint32 *FAT[2];
extern uint8 *ClusterData;
