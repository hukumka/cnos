#include "fat32_lowLevel.h"
#include <cn_string.h>
#include <memory.h>
#include <fs.h>

#define DIRECTORY_RECORD_SIZE 32
#define DIRECTORY_RECORDS_PER_SECTOR (512/DIRECTORY_RECORD_SIZE)
#define FAT_RECORDS_PER_SECTOR (512/sizeof(uint32))

uint32 currentDirectory;
uint32 currentCluster=0;
uint32 reloadFlag;

uint32 currentDirMember;

bool IsClusterIdCorrect( uint32 cl){
	return cl>2 && cl<0xf000000;
}

void Fat32_SelectDirectory( uint32 cluster ){
	if( currentCluster != cluster )
		reloadFlag = true;
	currentDirMember = 0;
	currentCluster = currentDirectory = cluster;
}

bool Fat32_GetDirNextMember( struct Fat32_DirMember * rezult ){
	if( currentDirMember==CurrentParams.SectorsPerCluster*DIRECTORY_RECORDS_PER_SECTOR ){
		currentCluster = Fat32_NextCluster( currentCluster );
		if( !IsClusterIdCorrect(currentCluster) ){
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
	rezult->attribByte = ClusterData[currentDirMember*DIRECTORY_RECORD_SIZE + 0xB];
	if( rezult->attrib.readonly && rezult->attrib.hidden && rezult->attrib.system && rezult->attrib.volumeId ){
		currentDirMember++;
		return Fat32_GetDirNextMember(rezult);
	}
	for(int i=0;i<11;++i)
		rezult->filename[i] = ClusterData[currentDirMember*DIRECTORY_RECORD_SIZE + i];
	if( rezult->filename[0] == 0 )
		return false;
	rezult->attribByte = ClusterData[currentDirMember*DIRECTORY_RECORD_SIZE + 0xB];
	rezult->firstCluster = *(uint16*)(ClusterData + currentDirMember*DIRECTORY_RECORD_SIZE + 0x14);
	rezult->firstCluster = rezult->firstCluster<<16 | *(uint16*)(ClusterData + currentDirMember*DIRECTORY_RECORD_SIZE + 0x1A);
	rezult->fileSize = *(uint32*)(ClusterData + currentDirMember*DIRECTORY_RECORD_SIZE + 0x1C);
	rezult->recordCluster = currentCluster;
	rezult->recordId = currentDirMember;
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

bool Fat32_SearchMemberOne( const char *name, struct Fat32_DirMember *rezult ){
	Fat32_SelectDirectory( currentDirectory );
	char shortName[11];
	Fat32_NameToShort(name, shortName);
	while( Fat32_GetDirNextMember( rezult ) ){
		// Проверка имени
		if( rezult->filename[0] == 0 )
			return false;
		if( rezult->filename[0] == 0xe5 )
			continue;
		if( strcmpl( rezult->filename, shortName, 11) == 0 )
			return true;
	}
	return false;
}

bool Fat32_SearchMember( uint32 baseDir,const char *path, struct Fat32_DirMember *rezult ){
	int i=0,j=0;
	char filename[256];
	Fat32_SelectDirectory( baseDir );
	for( ; path[i]; ++i){
		if( path[i]=='/' ){
			filename[j]=0;
			if( Fat32_SearchMemberOne( filename, rezult) )
				if( rezult->attrib.directory )
					Fat32_SelectDirectory( rezult->firstCluster );
				else
					return false;
			else 
				return false;
			j=0;
		}else{
			filename[j++] = path[i];
		}
	}
	if( j==0 )
		return true;
	filename[j]=0;
	return Fat32_SearchMemberOne( filename, rezult );
}

bool Fat32_open( uint32 baseDir, const char *path, FAT32_FILE *f ){
	struct Fat32_DirMember mem;
	if( Fat32_SearchMember( baseDir, path, &mem ) ){
		if( mem.attrib.directory )
			return false;
		f->buffer = (uint8*)Allocate( CurrentParams.SectorsPerCluster );
		f->fileSystem = 0xC;
		f->firstCluster = mem.firstCluster;
		f->fileSize = mem.fileSize;
		f->pointer = 0;
		f->recordCluster = mem.recordCluster;
		f->recordId = mem.recordId;
		return true;
	}else
		return false;
}

void Fat32_RewriteHead( FAT32_FILE *f ){
	uint32 cluster = f->recordCluster;
	Fat32_LoadCluster( cluster, ClusterData );
	*(uint32*)(ClusterData + DIRECTORY_RECORD_SIZE * f->recordId + 0x1C) = f->fileSize;
	Fat32_SaveCluster( cluster, ClusterData );
}

// Чтение из файла
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

// Запись в файл
uint32 Fat32_Write( FAT32_FILE *f, void *data, uint32 size ){
	uint32 cluster = f->firstCluster;
	uint32 clusterSize = 512 * CurrentParams.SectorsPerCluster;
	int i=0,j=2;
	if( !IsClusterIdCorrect(f->firstCluster) ){
		for(; FAT[0][j]!=0x0; ++j)
			if( j >= CurrentParams.SectorsPerFat*FAT_RECORDS_PER_SECTOR )
				return 0;
		f->firstCluster = j;
		FAT[0][j]=0xffffffff;
	}
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
	for( ; i<size; ++i ){
		if( (f->pointer + i)%clusterSize == 0 ){
			for( ; FAT[0][j]!=0x0 ; ++j ) 
				if( j >= CurrentParams.SectorsPerFat*FAT_RECORDS_PER_SECTOR ){
					goto label_exit;// 128 = 512/4 - количество записей в секторе fat тааблицы, все, таблица заполнена, места ббольше нетж
				}
			if( IsClusterIdCorrect( cluster ) )
			FAT[0][ cluster ] = j;
			FAT[0][ j ] = 0xffffffff;
			cluster = j;
		}
		f->buffer[ (f->pointer+i)%clusterSize ] = *( ((char*)data) + i);
	}
	label_exit:
	if( j!=2 )
		Fat32_SaveFat( 0 );
	Fat32_SaveCluster( cluster, f->buffer );
	if( f->fileSize < f->pointer + i ){
		f->fileSize = f->pointer + i;
		Fat32_RewriteHead( f );
	}
	return i;
}

void Fat32_Seek( FAT32_FILE *f, uint8 SeekType, int32 move){
	switch( SeekType ){
		case FILE_SEEK_FROMSTART:
			if( move<=0 ){
				f->pointer = 0;
				return;
			}
			if( move < f->fileSize )
				f->pointer = move;
			else
				f->pointer = f->fileSize;
			return;
		case FILE_SEEK_FROMEND:
			if( move<=0 ){
				f->pointer = f->fileSize;
				return;
			}
			if( move < f->fileSize )
				f->pointer = f->fileSize - f->pointer;
			else
				f->pointer = 0;
			return;
		case FILE_SEEK_OFFSET:
			if( f->pointer < -move )
				f->pointer = 0;
			else if( f->pointer > f->fileSize - move )
				f->pointer = f->fileSize;
			else
				f->pointer += move;
			return;
	}
}

void Fat32_FileClose( FAT32_FILE *f){
	Free( f->buffer, CurrentParams.SectorsPerCluster);
}

void Fat32_DeleteFile( struct Fat32_DirMember *f){
	uint32 cl = f->firstCluster;
	uint32 pcl;
	// Если это директория сперва рекурсивно вычищаем её содержимое
	if( f->attrib.directory ){
		struct Fat32_DirMember child;
		Fat32_SelectDirectory( cl );
		// Пропускаем указатели на текущий и родительский каталоги
		Fat32_GetDirNextMember( &child );
		Fat32_GetDirNextMember( &child );
		while( Fat32_GetDirNextMember( &child ) ){ // На самом деле лучше с конца, но это решение лучше инкапсулированно
			Fat32_DeleteFile( &child );
		}
	}
	// Помечаем кластеры файла как свободные
	while( IsClusterIdCorrect(cl) ){
		pcl = cl;
		cl = Fat32_NextCluster( cl );
		FAT[0][pcl] = 0;
	}
	
	if( f->recordCluster ){
		uint32 cl1 = Fat32_NextCluster( f->recordCluster );
		if( IsClusterIdCorrect(cl1)){
			bool flag = true;
			while( flag && IsClusterIdCorrect(cl1) ){
				Fat32_LoadCluster( cl1, ClusterData );
				for( int i=0; i<CurrentParams.SectorsPerCluster*16; ++i ){
					if( ClusterData[i*DIRECTORY_RECORD_SIZE] == 0xe5 )
						continue;
					if( ClusterData[i*DIRECTORY_RECORD_SIZE] != 0 )
						flag = false;
					break;
				}
				if( flag ){
					uint32 tmp = Fat32_NextCluster( cl1 );
					FAT[0][cl1] = 0;
					cl1 = tmp;
				}
			}
			FAT[0][f->recordCluster] = cl1;
		}
	}
	Fat32_SaveFat(0);
	
	Fat32_LoadCluster(f->recordCluster, ClusterData);
	// Последний элемент каталога.
	if( ClusterData[ (f->recordId+1)*DIRECTORY_RECORD_SIZE ] == 0 || ( IsClusterIdCorrect(FAT[0][f->recordCluster]) && f->recordId==CurrentParams.SectorsPerCluster*16 ) ){
		//Отмечаем его и все предыдущие как конец, в результате его получим, что элемент, следующий за последним использованным
		for( int i = f->recordId; i>=0 && ClusterData[i*DIRECTORY_RECORD_SIZE]==0xe5; --i)
			ClusterData[i*DIRECTORY_RECORD_SIZE]=0;
		return;
	}else{
		ClusterData[f->recordId*DIRECTORY_RECORD_SIZE] = 0xe5;
	}
	Fat32_SaveCluster( f->recordCluster, ClusterData);
}

bool Fat32_DirAddMember( struct Fat32_DirMember* dir, struct Fat32_DirMember* mem){
	uint32 cluster, nc=dir->firstCluster;
	int i,j;
	if( nc == 0){
		for(i=2; FAT[0][i]!=0; ++i )
			if( i == FAT_RECORDS_PER_SECTOR*CurrentParams.SectorsPerFat )
				return false;
	}
	do{
		cluster = nc;
		Fat32_LoadCluster( cluster, ClusterData);
		for( i=0; i<16*CurrentParams.SectorsPerCluster; ++i){
			if( ClusterData[i*DIRECTORY_RECORD_SIZE] == 0xe5 ){
				Fat32_LoadCluster(cluster, ClusterData);
				goto write;
			}else if( ClusterData[i*DIRECTORY_RECORD_SIZE]==0){
				if( i == DIRECTORY_RECORDS_PER_SECTOR*CurrentParams.SectorsPerCluster-1 ){
					for( j=2; FAT[0][j] != 0; ++j)
						if( j == CurrentParams.SectorsPerFat*FAT_RECORDS_PER_SECTOR )
							return false;
					FAT[0][cluster] = j;
					j=ClusterData[0];
					ClusterData[0]=0;
					Fat32_SaveCluster( j, ClusterData);
					ClusterData[0]=j;
					Fat32_SaveFat(0);
				}else{
					ClusterData[(i+1)*DIRECTORY_RECORD_SIZE]=0;	
				}
				goto write;
			}
		}
		nc = Fat32_NextCluster(cluster);
	}while( IsClusterIdCorrect(nc) );
	for( j=2; FAT[0][j] != 0; ++j)
		if( j == CurrentParams.SectorsPerFat*FAT_RECORDS_PER_SECTOR )
			return false;
	FAT[0][cluster] = j;
	Fat32_SaveFat(0);
	cluster=j;
	Fat32_LoadCluster(cluster, ClusterData);
	i = 0;
	ClusterData[DIRECTORY_RECORD_SIZE]=0;
write:
	for(j=0; j<11; ++j)
		ClusterData[i*DIRECTORY_RECORD_SIZE+j] = mem->filename[j];
	ClusterData[i*32+0xB] = mem->attribByte;
	*(uint16*)(ClusterData + i*DIRECTORY_RECORD_SIZE + 0x14) = (uint16)(mem->firstCluster>>16);
	*(uint16*)(ClusterData + i*DIRECTORY_RECORD_SIZE + 0x1A) = (uint16)(mem->firstCluster);
	*(uint32*)(ClusterData + i*DIRECTORY_RECORD_SIZE + 0x1C) = (uint32)(mem->fileSize);
	Fat32_SaveCluster( cluster, ClusterData);
	return true;;
}

bool Fat32_CreateFile( const char *path, const char *filename, bool dir){
	struct Fat32_DirMember mem,new;
	if( !Fat32_SearchMember( CurrentParams.RootDirCluster, path, &mem ) )
		return false;
	if( !mem.attrib.directory )	
		return false;
	Fat32_NameToShort(filename, new.filename);
	new.attrib.readonly = 0;
	new.attrib.hidden = 0;
	new.attrib.system = 0;
	new.attrib.volumeId = 0;
	new.attrib.directory = (dir!=0);
	new.attrib.archive = 0;
	new.attrib.unused = 0;
	new.fileSize = 0;
	new.firstCluster = 0;
	return Fat32_DirAddMember( &mem, &new);
}
