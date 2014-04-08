#pragma once

#define FILE_SEEK_FROMSTART 0
#define FILE_SEEK_FROMEND 1
#define FILE_SEEK_OFFSET 2
typedef enum{
	FROM_START,
	FROM_OFFSET,
	FROM_END,
} FILE_SEEK_TYPE;

typedef struct{
	uint8 DRIVE_ID;
	uint32 startIndex;
	uint32 size;
	uint32 reversed;
	uint32 pointer;
} FILE;

bool SelectDisk( uint8 id );
void InitFilesystemManager();

FILE * fopen(const char *filePath);
int fwrite( void *destination, int elemSize, int elemCount, FILE *file);
int fread( void *source, int elemSize, int elemCount, FILE *file);
int fseek( FILE *file, int offset, FILE_SEEK_TYPE type);
