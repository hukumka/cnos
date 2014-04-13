#include <memory.h>

#define KB 0x400
#define MB (0x400*KB)

#define MEMORY_TABLE_BASE ((uint8*)0x0)
#define AVAILABLE_MEMORY_BASE 0x150000
#define AVAILABLE_MEMORY_SIZE (80*MB/BLOCK_SIZE) // Всего 80 мб
#define TABLE_SIZE (AVAILABLE_MEMORY_SIZE/8)

#define FREE 0
#define BUSY 1

#define NO_SUITABLE_BLOCK -1
#define NO_SUITABLE_SEGMENT -1

bool IsBlockFree( int id );
void SetBlockStateFree( int id);
void SetBlockStateBusy( int id );
int SearchFreeSegment(int size);
void SetBlocksSegmentFree(int firstId, int count);
void SetBlocksSegmentBusy(int firstId, int count);
void* TableIndexToPointer(int index);
int PointerToTableIndex( void *pointer );

//====================
// ПРЕДОСТАВЛЯЕМОЕ API

// Инициализация таблицы памяти
void InitMemory(){
	for( int i=0; i<TABLE_SIZE; ++i)
		MEMORY_TABLE_BASE[i]=FREE;
}
// Выделение памяти
void* Allocate( int size ){
	int spaceFirstBlockId = SearchFreeSegment( size );
	if( spaceFirstBlockId == NO_SUITABLE_SEGMENT ){
		printf("Error. Unable to allocate memory.\n");
		return (void*)0x0;
	}
	SetBlocksSegmentBusy( spaceFirstBlockId, size );
	return TableIndexToPointer(spaceFirstBlockId);
}
// Освобождение памяти
bool Free( void *pointer, int size ){
	int tableIndex = PointerToTableIndex( pointer );
	if( tableIndex == NO_SUITABLE_BLOCK ){
		printf("Error. Current memory segment cannoot be free.\n");
		return false;
	}
	SetBlocksSegmentFree( tableIndex, size);
	return true;
}

//========================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ

// Поиск свободного отрезка длинны size
int SearchFreeSegment(int size){
	int i, foundedFreeSegmentSize;
	for( i=0; i<AVAILABLE_MEMORY_SIZE; ++i){
		if( IsBlockFree(i) ){
			foundedFreeSegmentSize++;
			if( foundedFreeSegmentSize == size )
				return i-size+1;
		}else
			foundedFreeSegmentSize=0;
	}
	return NO_SUITABLE_SEGMENT;
}

// Установка состояния всех блоков отрезка [firstId, firstId+count] как свободные
void SetBlocksSegmentFree(int firstId, int count){
	for( int i=0; i<count; ++i )
		SetBlockStateFree( firstId+i );
}
// Установка состояния всех блоков отрезка [firstId, firstId+count] как занятые
void SetBlocksSegmentBusy(int firstId, int count){
	for( int i=0; i<count; ++i )
		SetBlockStateBusy( firstId+i );
}

// Преобразование номера блока в таблице в адрес
inline void* TableIndexToPointer(int index){
	return (void*)AVAILABLE_MEMORY_BASE + index*BLOCK_SIZE;
}
// Преобразование адреса в номер в таблицы
inline int PointerToTableIndex( void *pointer ){
	uint32 pointerOffset = (uint32)pointer - AVAILABLE_MEMORY_BASE;
	if( pointerOffset % BLOCK_SIZE != 0 )
		return NO_SUITABLE_BLOCK;
	int tableIndex = pointerOffset/BLOCK_SIZE;
	if( tableIndex >= AVAILABLE_MEMORY_SIZE )
		return NO_SUITABLE_BLOCK;
	return tableIndex;
}
//Свободен ли блок id
bool IsBlockFree( int id ){
	return MEMORY_TABLE_BASE[id/8]&(1<<(id%8)); 
}
// Установка состояния блока id в свободно
void SetBlockStateFree( int id){
	MEMORY_TABLE_BASE[id/8] |= (1<<(id%8));
}
// Установка состояния блока id в занято
void SetBlockStateBusy( int id ){
	MEMORY_TABLE_BASE[id/8] &= !(1<<(id%8));
}



