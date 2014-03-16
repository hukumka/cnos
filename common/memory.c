#include <memory.h>

#define KB 0x400
#define MB (0x400*KB)

#define BASE ((uint8*)0x0)
#define FROM 0x150000
#define SIZE (80*MB/BLOCK_SIZE) // Всего 80 мб
#define TABLESIZE (SIZE/8)

void InitMemoryTable(){
	for( int i=0; i<TABLESIZE; ++i)
		BASE[i]=0xff;
}

bool IsBlockFree( int id ){
	return BASE[id/8]&(1<<(id%8)); 
}
void InvBlockState( int id ){
	BASE[id/8] ^= (1<<(id%8));
}

void* Allocate( int size ){
	// Ищем свободное место
	int i,j;
	int fsize=0;
	for( i=0; i<SIZE; ++i){
		if( IsBlockFree(i) ){
			fsize++;
			if( fsize == size ){
				// Нашлась
				i-=size-1;
				for(j=0;j<size;++j)
					InvBlockState(i+j);
				return (void*)(FROM+BLOCK_SIZE*i);
			}
		}else
			fsize=0;
	}
	return (void*)0x0;
}

bool Free( void *block, int size ){
	uint32 i = (uint32)block - FROM;
	if( i % BLOCK_SIZE ) // Адрес не кратный BLOCK_SIZE не может быть корректен
		return false;
	i/=BLOCK_SIZE;
	if( i>=SIZE )
		return false;
	uint32 to=i+size;
	while(i<to){
		if( IsBlockFree(i) )
			return false;
		i++;
	}
	for(i-=size;i<to;++i)
		InvBlockState(i);
	return true;
}
