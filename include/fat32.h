#pragma once
#include <types.h>

#define PATH_INVALID 1

struct Fat32_DirMember{
	char filename[11];
	union{
		struct{
			uint8 readonly: 1;
			uint8 hidden: 1;
			uint8 system: 1;
			uint8 volumeId: 1;
			uint8 directory: 1;
			uint8 archive: 1;
			uint8 unused: 2;
		}attrib;
		uint8 attribByte;
	};
	uint32 firstCluster;
	uint32 fileSize;
};

typedef struct{
	uint8 fileSystem;
	char filename[11];
	uint32 firstCluster;
	uint8 *buffer;
	uint32 fileSize;
	uint32 pointer;
	uint32 recordCluster;
	uint8 recordId;
} FAT32_FILE;


bool Fat32_GetDirNextMember( struct Fat32_DirMember * rezult );
void Fat32_SelectDirectory( uint32 cluster );
void Fat32_NameToShort( const char *name, char * rezult);
bool Fat32_SearchMember( const char *name, struct Fat32_DirMember *rezult );
bool Fat32_Init( uint32 );
bool Fat32_open( uint32, const char *,FAT32_FILE *);

uint32 Fat32_Read( FAT32_FILE *f, void *data, uint32 size);
uint32 Fat32_Write( FAT32_FILE *f, void *data, uint32 size);
