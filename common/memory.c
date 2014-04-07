#include <memory.h>

#define KB 0x400
#define MB (0x400*KB)

#define MEMORY_TABLE_BASE ((uint8*)0x0)
#define AVAILABLE_MEMORY_BASE 0x150000
#define AVAILABLE_MEMORY_SIZE (80*MB/BLOCK_SIZE) // Всего 80 мб
#define TABLE_SIZE (AVAILABLE_MEMORY_SIZE/8)

void InitMemoryTable(){
	for( int i=0; i<TABLE_SIZE; ++i)
		MEMORY_TABLE_BASE[i]=0xff;
}

bool IsBlockFree( int id ){
	return MEMORY_TABLE_BASE[id/8]&(1<<(id%8)); 
}
void InvertBlockState( int id ){
	MEMORY_TABLE_BASE[id/8] ^= (1<<(id%8));
}

void* Allocate( int size ){
	int i,j;
	int foundedFreeSize=0;
	// Ищем свободное место
	for( i=0; i<AVAILABLE_MEMORY_SIZE; ++i){
		if( IsBlockFree(i) ){
			foundedFreeSize++;
			if( foundedFreeSize == size ){
				// Нашлась
				i-=size-1;
				for(j=0;j<size;++j)
					InvertBlockState(i+j);
				return (void*)(AVAILABLE_MEMORY_BASE+BLOCK_SIZE*i);
			}
		}else
			foundedFreeSize=0;
	}
	return (void*)0x0;
}

bool Free( void *address, int size ){
	uint32 addressOffsetFromBase = (uint32)address - AVAILABLE_MEMORY_BASE;
	if( addressOffsetFromBase % BLOCK_SIZE ) // Адрес не кратный BLOCK_AVAILABLE_MEMORY_SIZE не может быть корректен
		return false;
	uint32 blockIndex = addressOffsetFromBase / BLOCK_SIZE;
	if( blockIndex>=AVAILABLE_MEMORY_SIZE )
		return false;
	uint32 to=blockIndex+size;
	while(blockIndex<to){
		if( IsBlockFree(blockIndex) )
			return false;
		blockIndex++;
	}
	for(blockIndex-=size; blockIndex<to; ++blockIndex)
		InvertBlockState(blockIndex);
	return true;
}
