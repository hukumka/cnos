#include <fat32.h>
#include <memory.h>
#include <ata.h>
#include <string_us.h>

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

#define FAT_CLUSTER_ERROR 1
#define FAT_FILE_END 0xffffffff
#define FAT_CLUSTER_FREE 0x0

// Данные по разделам
uint32 FS_LBABeginSector[8];
uint8 FS_Type[8];
uint8 FAT32VolumeId[512];

// Данные по используемому разделу
uint8 Fat32CurrentFileSystem=0;
uint32 Fat32Current_BeginFat;
uint32 Fat32Current_ClusterBeginLBA;
uint8 Fat32Current_SectorPerCluster;
uint32 Fat32Current_SectorPerFat;
uint32 Fat32Current_RootDirecoryCluster;

uint32 CurrentCluster;
uint32 CurrentDirectoryFirstCluster;
bool relocateFlag=true;

#define BUFFER_COUNT 2
#define BUFFER_FAT1 0
#define BUFFER_CLUSTER 1
void* buffer[2];
int OldBufferSize[2];

void PrepareBuffers(){
	for( int i=0; i<BUFFER_COUNT; ++i)
		buffer[i] = 0;
}
void ReloadBuffers(){
	for( int i=0; i<BUFFER_COUNT; ++i){
		if( buffer[i] != 0 )
			Free( buffer[i], OldBufferSize[i]);
		int size;
		switch( i ){
			case BUFFER_FAT1:
				size=Fat32Current_SectorPerFat;
				break;
			case BUFFER_CLUSTER:
				size=Fat32Current_SectorPerCluster;
				break;
		}
		buffer[i] = Allocate( size );
		OldBufferSize[i] = size;
	}
}


// Загружает таблицы разделов с диска
void LoadPartitions(){
	ATAPIO_Read(0,1,master,FAT32VolumeId);
	for(int i=0;i<4;++i){
		FS_Type[i] = BOOTSECTOR[ PARTITION_BASE + i*PARTITION_SIZE + PR_TYPE_OFFSET ];
		FS_LBABeginSector[i] = *(uint32*)(BOOTSECTOR + PARTITION_BASE + i*PARTITION_SIZE + PR_LBABEGIN_OFFSET);
	}
	PrepareBuffers();
}

// Загружает FAT таблицу в память
bool LoadFAT(uint8 id, void *fat){
	if(id!=0)
		id=1;
	return ATAPIO_Read( Fat32Current_BeginFat + id*Fat32Current_SectorPerFat, Fat32Current_SectorPerFat,master, fat);
}
// Сохраняет FAT  таблицу из файла
bool SaveFat(uint8 id, void *fat){
	if(id!=0)
		id=1;
	return ATAPIO_Write( Fat32Current_BeginFat + id*Fat32Current_SectorPerFat, Fat32Current_SectorPerFat,master, fat);
}

// Подготовка раздела к использованию
bool SelectPartition(uint8 id){
	ATAPIO_Read( FS_LBABeginSector[id], 1, (drive)(id/4), FAT32VolumeId );
	if( *(uint16*)(FAT32VolumeId	+ FS_VOLUMEID_BYTE_PER_SECTOR_OFFSET) != SECTOR_SIZE ){ // Проверяем размер сектора
		Fat32CurrentFileSystem=0;
		return false;
	}
	if( *(uint8*)(FAT32VolumeId + FS_VOLUMEID_NUMBER_OF_FATS_OFFSET) != 2 ){ // Проверяем количество FAT таблиц
		Fat32CurrentFileSystem=0;
		return false;
	}
	if( *(uint16*)(FAT32VolumeId + FS_VOLUMEID_SIGNATURE_OFFSET) != 0xAA55 ){ // Проверяем сигнатуру
		Fat32CurrentFileSystem=0;
		return false;
	}
	Fat32Current_SectorPerFat = *(uint32*)(FAT32VolumeId + FS_VOLUMEID_SECTORS_PER_FAT_OFFSET);
	Fat32CurrentFileSystem=0x80 | id;
	Fat32Current_BeginFat = FS_LBABeginSector[id] + *(uint16*)(FAT32VolumeId + FS_VOLUMEID_NUMBER_OF_REVERSED_SECTORS_OFFSET);
	Fat32Current_ClusterBeginLBA = Fat32Current_BeginFat + FAT32VolumeId[ FS_VOLUMEID_NUMBER_OF_FATS_OFFSET ]*Fat32Current_SectorPerFat;
	Fat32Current_SectorPerCluster = FAT32VolumeId[ FS_VOLUMEID_SECTOR_PER_CLUSTER_OFFSET ];
	Fat32Current_RootDirecoryCluster = *(uint32*)(FAT32VolumeId + FS_VOLUMEID_ROOT_DIRECTORY_FIRST_CLUSTER_OFFSET);
	CurrentCluster = Fat32Current_RootDirecoryCluster;
	ReloadBuffers();
	LoadFAT( 0, buffer[BUFFER_FAT1] );
	return true;
}


// Используя FAT таблицу определяет номер следующего кластера
uint32 NextCluster(uint32 cluster){
	if( cluster < 2 || cluster > Fat32Current_SectorPerFat*128)
		return FAT_CLUSTER_ERROR;
	return *((uint32*)buffer[BUFFER_FAT1] + cluster);
}

// Читает кластер с номером id в область памяти data
inline bool LoadCluster(uint32 id, void *data){
	uint32 sectors_start = Fat32Current_ClusterBeginLBA + (id-2) * Fat32Current_SectorPerCluster;
	if( (sectors_start>>28) != 0 ){
		return false;
	}
	return ATAPIO_Read( sectors_start, Fat32Current_SectorPerCluster, master, data);
}
// Пишет в кластер с номером id из область памяти data
inline bool SaveCluster(uint32 id, void *data){
	uint32 sectors_start = Fat32Current_ClusterBeginLBA + (id-2) * Fat32Current_SectorPerCluster;
	if( (sectors_start>>28) != 0 ){
		return false;
	}
	return ATAPIO_Write( sectors_start, Fat32Current_SectorPerCluster, master, data);
}

bool GetFat32DirMember( void *cluster, uint32 fileId, struct Fat32_DirMember *rezult ){
	for(int i=0;i<11;++i){
		rezult->filename[i] = *(char*)(cluster + 32*fileId + i);
	}
	rezult->attribByte = *(uint8*)(cluster + 32*fileId + 0xb);
	// Получаем номер первого сектора
	rezult->firstCluster = (uint32) *(uint16*)(cluster + 32*fileId +0x14); // Верх первого кластера
	rezult->firstCluster = (rezult->firstCluster<<16) | (uint32)*(uint16*)(cluster + 32*fileId + 0x1A);
	rezult->fileSize = *(uint32*)(cluster + 32*fileId + 0x1C);
	Free(cluster, SECTOR_SIZE*Fat32Current_SectorPerCluster);
	return true;
}

bool SearchFat32DirMember( uint32 dirCluster, const char* filename, struct Fat32_DirMember *rezult){
	SelectDir( dirCluster );
	do{
		
	}
}


void SelectDir( uint32 dirFirstCluster ){
	CurrentCluster = CurrentDirectoryFirstCluster = dirFirstCluster;
	relocateFlag=true;
}

bool NextDirFileMember(struct Fat32_DirMember *rezult){
	static int pos;
	if( relocateFlag ){
		pos=0;
		LoadCluster( CurrentCluster, buffer[BUFFER_CLUSTER]);
		relocateFlag = false;
	}
	if( pos == SECTOR_SIZE*Fat32Current_SectorPerCluster/32 ){
		CurrentCluster = NextCluster( CurrentCluster );
		pos=0;
		LoadCluster( CurrentCluster, buffer[BUFFER_CLUSTER]);
	}
	bool s = GetFat32DirMember( buffer[BUFFER_CLUSTER], pos, rezult);
	if( !s )
		return false;
	if( rezult->filename[0] == 0 )
		return false;
	if( rezult->attrib.readonly && rezult->attrib->hidden && rezult->attrib.system && rezult->attrib.volumeId ){
		pos++;
		return NextDirFileMember( rezult );
	}
	pos++;
	return true;
}

void GotIntoRoot(){
	SelectDir( Fat32Current_RootDirecoryCluster );
}

/*
uint32 GetFileClusterByPath(void *fat, const char * path, uint32 baseDir){
	char dirname[11];
	int j=0;
	struct Fat32_DirMember dirMember;
	for( int i=0; path[i]; ++i){
		if( path[i]=='/' ){
			j=0;
			SearchFat32DirMember(fat, baseDir, dirname, &dirMember);
			if( !dirMember.attrib.directory )
				return PATH_INVALID;
			baseDir = dirMember.firstCluster;
		}
	}
}*/
