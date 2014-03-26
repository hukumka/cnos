#include "fat32_lowLevel.h"
#include <cn_string.h>
#include <memory.h>

uint32 currentDirectory;
uint32 currentCluster=0;
uint32 reloadFlag;

uint32 currentDirMember;

void Fat32_SelectDirectory( uint32 cluster ){
	if( currentCluster != cluster )
		reloadFlag = true;
	currentDirMember = 0;
	currentCluster = currentDirectory = cluster;
}

bool Fat32_GetDirNextMember( struct Fat32_DirMember * rezult ){
	if( currentDirMember==CurrentParams.SectorsPerCluster*(512/32) ){
		currentCluster = Fat32_NextCluster( currentCluster );
		if( currentCluster == 0xffffffff ){
			Fat32_SelectDirectory( currentDirectory );
			return false;
		}
		reloadFlag = true;
	}
	if( reloadFlag ){
		currentDirMember = 0;
		Fat32_LoadCluster( currentCluster, ClusterData );
		reloadFlag = false;
	}
	rezult->attribByte = ClusterData[currentDirMember*32 + 0xB];
	if( rezult->attrib.readonly && rezult->attrib.hidden && rezult->attrib.system && rezult->attrib.volumeId ){
		currentDirMember++;
		return Fat32_GetDirNextMember(rezult);
	}
	for(int i=0;i<11;++i)
		rezult->filename[i] = ClusterData[currentDirMember*32 + i];
	if( rezult->filename[0] == 0 )
		return false;
	rezult->attribByte = ClusterData[currentDirMember*32 + 0xB];
	rezult->firstCluster = *(uint16*)(ClusterData + currentDirMember*32 + 0x14);
	rezult->firstCluster = rezult->firstCluster<<16 | *(uint16*)(ClusterData + currentDirMember*32 + 0x1A);
	rezult->fileSize = *(uint32*)(ClusterData + currentDirMember*32 + 0x1C);
	currentDirMember++;
	return true;
}

void Fat32_NameToShort( const char *name, char * rezult){
	int i,j;
	for( i=0; i<8 && name[i]!='.' && name[i]!=0; ++i){
		rezult[i] = name[i];
		if( rezult[i]>='a' && rezult[i]<='z' )
			rezult[i] += 'A'-'a';
	}
	if( i==8 )
		rezult[7]='~';
	for(j=i;j<8;++j)
		rezult[j]=' ';
	for( ; name[i]!='.' && name[i]!=0; ++i)
		;
	if( name[i] == '.' ){
		++i;
		for( j=8; j<11 && name[i]!=0; ++j, ++i){
			rezult[j]=name[i];
			if( rezult[j]>='a' && rezult[j]<='z' )
				rezult[j] += 'A'-'a';
		}
	}else{
		for( j=8; j<11; ++j )
			rezult[j]=' ';
	}
}

bool Fat32_SearchMember( const char *name, struct Fat32_DirMember *rezult ){
	Fat32_SelectDirectory( currentDirectory );
	char shortName[11];
	Fat32_NameToShort(name, shortName);
	while( Fat32_GetDirNextMember( rezult ) ){
		// Проверка имени
		if( strcmpl( rezult->filename, shortName, 11) == 0 )
			return true;
	}
	return false;
}

bool Fat32_open( uint32 baseDir, const char *path, FAT32_FILE *f ){
	int i=0,j=0;
	char filename[256];
	struct Fat32_DirMember mem;
	Fat32_SelectDirectory( baseDir );
	for( ; path[i]; ++i){
		if( path[i]=='/' ){
			filename[j]=0;
			if( Fat32_SearchMember( filename, &mem) )
				if( mem.attrib.directory )
					Fat32_SelectDirectory( mem.firstCluster );
				else
					return false;
			else 
				return false;
			j=0;
		}else{
			filename[j++] = path[i];
		}
	}
	filename[j]=0;
	if( Fat32_SearchMember( filename, &mem ) ){
		if( mem.attrib.directory )
			return false;
		f->buffer = (uint8*)Allocate( CurrentParams.SectorsPerCluster );
		f->fileSystem = 0xC;
		f->firstCluster = mem.firstCluster;
		f->fileSize = mem.fileSize;
		f->pointer = 0;
		f->recordId = currentDirMember - 1;
		f->recordCluster = currentCluster;
		return true;
	}else
		return false;
}

void Fat32_RewriteHead( FAT32_FILE *f ){
	uint32 cluster = f->recordCluster;
	Fat32_LoadCluster( cluster, ClusterData );
	*(uint32*)(ClusterData + 32 * f->recordId + 0x1C) = f->fileSize;
	Fat32_SaveCluster( cluster, ClusterData );
}

uint32 Fat32_Read( FAT32_FILE *f, void *data, uint32 size ){
	uint32 cluster = f->firstCluster;
	uint32 clusterSize = 512 * CurrentParams.SectorsPerCluster;
	int i;
	for( i=clusterSize; i<f->pointer; i+=clusterSize)
		cluster = Fat32_NextCluster( cluster );
	Fat32_LoadCluster( cluster,  f->buffer );
	for( i=0; (f->pointer + i < f->fileSize) && (i < size); i++ ){
		if( (f->pointer+i) % clusterSize == 0 && i!=0 ){
			cluster = Fat32_NextCluster( cluster );
			Fat32_LoadCluster( cluster, f->buffer);
		}
		*( ((char*)data) + i) = f->buffer[ (f->pointer+i)%clusterSize ];
	}
	f->pointer += i;
	return i;
}

uint32 Fat32_Write( FAT32_FILE *f, void *data, uint32 size ){
	uint32 cluster = f->firstCluster;
	uint32 clusterSize = 512 * CurrentParams.SectorsPerCluster;
	int i;
	for( i=clusterSize; i<f->pointer; i+=clusterSize)
		cluster = Fat32_NextCluster( cluster );
	Fat32_LoadCluster( cluster, f->buffer );
	for( i=0; (f->pointer+i < f->fileSize) && ( i<size ); ++i ){
		if( (f->pointer+i) % clusterSize == 0 && i!=0 ){
			Fat32_SaveCluster( cluster, f->buffer);
			cluster = Fat32_NextCluster( cluster );
			if( f->pointer + i + clusterSize > size ){ // Если следующий кластер не полностью перезаписан, прочесть его перед записью
				Fat32_LoadCluster( cluster, f->buffer );
			}
		}
		f->buffer[ (f->pointer+i)%clusterSize ] = *( ((char*)data) + i);
	}
	int j=2;
	for( ; i<size; ++i ){
		if( (f->pointer + i)%clusterSize == 0 ){
			Fat32_SaveCluster( cluster, f->buffer );
			for( ; FAT[0][j]!=0x0 ; ++j ) 
				if( j >= CurrentParams.SectorsPerFat*128 ){
					goto label_exit;// 128 = 512/4 - количество записей в секторе fat тааблицы, все, таблица заполнена, места ббольше нетж
				}
			FAT[0][ cluster ] = j;
			FAT[0][ j ] = 0xffffffff;
			cluster = j;
		}
		f->buffer[ (f->pointer+i)%clusterSize ] = *( ((char*)data) + i);
	}
	label_exit:
	Fat32_SaveCluster( cluster, f->buffer );
	if( f->fileSize < f->pointer + i ){
		f->fileSize = f->pointer + i;
		if( j!=2 )
			Fat32_SaveFat( 0 );
		Fat32_RewriteHead( f );
	}
	return i;
}
