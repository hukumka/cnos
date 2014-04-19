#include <types.h>

#define BLOCK_SIZE 0x400

void InitMemory();
void * Allocate( int blockcount );
bool Free( void *addr, int size);
