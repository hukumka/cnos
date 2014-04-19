#include <run.h>

#define ELFHEADER_TYPE_OFFSET 0x10
#define ELFHEADER_ENTERPOINT_OFFSET 0x18
#define ELFHEADER_CODEHEADEROFFSET_OFFSET 0x1C
#define ELFHEADER_CODEHEADERCOUNT_OFFSET 0x2C

#define CODEHEADER_SIZE 0x20
#define CODEHEADER_TYPE_OFFSET 0x0
#define CODEHEADER_FILEOFFSET_OFFSET 0x4
#define CODEHEADER_ADDRESS_OFFSET 0x8
#define CODEHEADER_PHYSICALASIZE_OFFSET 0x10
#define CODEHEADER_VIRTUALSIZE_OFFSET 0x14

#define FILE_TYPE_EXECTABLE 2
#define CODESECTION_LOADABLE 1

#define LOADPROGSECTION_ERROR -1

static bool TestElfFileHeader( FAT32_FILE *file );
static bool TestExectableFileType( FAT32_FILE *elfFile );
static uint32 GetExectableEnterPointAddress( FAT32_FILE *elfFile );
static uint32 GetCodeHeaderTableOffset( FAT32_FILE *elfFile, int index );
static uint16 GetCodeHeadersCount( FAT32_FILE *elfFile );
static int LoadProgramSectionAndRun( FAT32_FILE *elfFile, int index, int enterPoint );

int CreateProcess( const char *filePath ){
	FAT32_FILE file;
	if( ! Fat32_open( filePath, &file) )
		return false;
	if( ! TestElfFileHeader( &file ) )
		return false;
	if( ! TestExectableFileType( &file ) )
		return false;
	uint32 enterPointAddr = GetExectableEnterPointAddress( &file );
	uint16 codeHeadersCount = GetCodeHeadersCount( &file );
	int rezult;
	for( int i=0; i<codeHeadersCount; ++i){
		rezult = LoadProgramSectionAndRun( &file, i, enterPointAddr);
		if( rezult == LOADPROGSECTION_ERROR ){
			printf("Run error\n");
		}else{
			return rezult; // Загружаем только первую подошедшую секцию. Плачем от безысходности
		}
	}
	return rezult;
}

bool TestElfFileHeader( FAT32_FILE *file ){
	char header[4];
	int readedBytes = Fat32_Read( file, header, sizeof(header));
	if( readedBytes != sizeof(header) ){
		return false;
	}
	return strcmpl(header,"\177ELF", sizeof(header))==0;
}  

bool TestExectableFileType( FAT32_FILE *elfFile ){
	uint16 type;
	Fat32_Seek(elfFile, FILE_SEEK_FROMSTART, ELFHEADER_TYPE_OFFSET);
	int readedBytes = Fat32_Read( elfFile, &type, sizeof(uint16) );
	if( readedBytes != sizeof(uint16) ){
		return false;
	}
	return type == FILE_TYPE_EXECTABLE;
} 

uint32 GetExectableEnterPointAddress( FAT32_FILE *elfFile ){
	uint32 EnterPointAddress;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, ELFHEADER_ENTERPOINT_OFFSET);
	int readedBytes = Fat32_Read( elfFile, &EnterPointAddress, sizeof(uint32) );
	if( readedBytes != sizeof(uint32) ){
		return false;
	}
	return EnterPointAddress;
}


uint32 GetCodeHeaderTableOffset( FAT32_FILE *elfFile, int index ){
	uint32 offset;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, ELFHEADER_CODEHEADEROFFSET_OFFSET);
	int readedBytes = Fat32_Read( elfFile, &offset, sizeof(uint32) );
	if( readedBytes != sizeof(uint32) ){
		return false;
	}
	return offset + CODEHEADER_SIZE*index;
}
 
uint16 GetCodeHeadersCount( FAT32_FILE *elfFile ){
	uint16 count;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, ELFHEADER_CODEHEADERCOUNT_OFFSET);
	int readedBytes = Fat32_Read( elfFile, &count, sizeof(uint16) );
	if( readedBytes != sizeof(uint16) ){
		return false;
	}
	return count;
}

/* LoadProgramSections( FAT32_FILE *elfFile, int index ), и её вспомогательные функции */
static bool TestCodeHeaderType( FAT32_FILE *elfFile, int tableOffset );
static uint32 GetCodeBlockPhysSize( FAT32_FILE *elfFile, int tableOffset);
static uint32 GetCodeBlockFileOffset( FAT32_FILE *elfFile, int tableOffset );
static uint32 GetCodeBlockMemBaseAddress( FAT32_FILE *elfFile, int tableOffset );
static uint32 GetCodeBlocVirtualSize( FAT32_FILE *elfFile, int tableOffset );
bool ReadCode( FAT32_FILE *elfFile, int fileDataOffset, int fileDataSize, void *destination );

static int LoadProgramSectionAndRun( FAT32_FILE *elfFile, int index, int enterPoint ){
	int tableOffset = GetCodeHeaderTableOffset( elfFile, index );
	if( ! TestCodeHeaderType( elfFile, tableOffset) ){
		return false;
	}
	uint32 blockOffsetInFile = GetCodeBlockFileOffset( elfFile, tableOffset );
	uint32 blockSizeInFile = GetCodeBlockPhysSize( elfFile, tableOffset );
	if( blockSizeInFile == 0 ){
		return false;
	}
	uint32 blockBaseAddress = GetCodeBlockMemBaseAddress( elfFile, tableOffset );
	if( blockBaseAddress != 0 ){
		return false;
	}
	uint32 blockVirtualSize = GetCodeBlocVirtualSize( elfFile, tableOffset );
	
	blockVirtualSize = (blockVirtualSize|0xfff) + 1; // Округляем вверх до размера кратного 4096 (для создания дескриптора сегмента с флагом гранулярности)
	void *baseAddressInFlat = Allocate( blockVirtualSize/BLOCK_SIZE);
	ReadCode( elfFile, blockOffsetInFile, blockSizeInFile, baseAddressInFlat );
	uint16 dataSegment = NewSuperUserDataDescriptor( (uint32)baseAddressInFlat, blockVirtualSize );
	uint16 codeSegment = NewSuperUserCodeDescriptor( (uint32)baseAddressInFlat, blockVirtualSize );

	
	asm( "mov %%ax, %%ds\n" :: "a"(dataSegment) );
	longcall( codeSegment, enterPoint);
	int rezult;
	asm("mov %%eax, %0":"=r"(rezult));
	return rezult;
}



static bool TestCodeHeaderType( FAT32_FILE *elfFile, int tableOffset ){
	uint32 type;
	uint32 offset = tableOffset + CODEHEADER_TYPE_OFFSET;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, offset );
	int readedBytes = Fat32_Read( elfFile, &type, sizeof(uint32) );
	if( readedBytes != sizeof(uint32) )
		return false;
	return type == CODESECTION_LOADABLE;
} 

static uint32 GetCodeBlockFileOffset( FAT32_FILE *elfFile, int tableOffset ){
	int headerValueOffset = tableOffset + CODEHEADER_FILEOFFSET_OFFSET;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, headerValueOffset);
	uint32 codeBlockOffset;
	Fat32_Read( elfFile, &codeBlockOffset, sizeof(uint32) );
	return codeBlockOffset;
} 

static uint32 GetCodeBlockPhysSize(  FAT32_FILE *elfFile, int tableOffset){
	uint32 offset = tableOffset + CODEHEADER_PHYSICALASIZE_OFFSET;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, offset);
	uint32 size;
	int readedBytes = Fat32_Read( elfFile, &size, sizeof(uint32) );
	if( readedBytes != sizeof(uint32) )
		return 0;
	return size;
} 

static uint32 GetCodeBlockMemBaseAddress( FAT32_FILE *elfFile, int tableOffset ){
	int offset = tableOffset + CODEHEADER_ADDRESS_OFFSET;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, offset);
	uint32 address;
	Fat32_Read( elfFile, &address, sizeof(uint32) );
	return address;
}

static uint32 GetCodeBlocVirtualSize( FAT32_FILE *elfFile, int tableOffset ){
	uint32 size;
	int offset = tableOffset + CODEHEADER_VIRTUALSIZE_OFFSET;
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, offset);
	int readedBytes = Fat32_Read( elfFile, &size, sizeof(uint32) );
	if( readedBytes != sizeof(uint32) )
		return 0;
	return size;
}

bool ReadCode( FAT32_FILE *elfFile, int fileDataOffset, int fileDataSize, void *destination ){
	Fat32_Seek( elfFile, FILE_SEEK_FROMSTART, fileDataOffset);
	int readedBytes = Fat32_Read( elfFile, destination, fileDataSize);
	return readedBytes == fileDataSize;
}
// block end
