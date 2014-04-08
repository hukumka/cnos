#include <ata.h>
#include <macros.h>

#define ATAPIO_BASE_ADDR 0x1F0
#define ATAPIO_PORT_OFFSET_DATA 0x0
#define ATAPIO_PORT_OFFSET_ERROR 0x1
#define ATAPIO_PORT_OFFSET_FEATURES 0x1
#define ATAPIO_PORT_OFFSET_SECTORCOUNT 0x2
#define ATAPIO_PORT_OFFSET_LBA_0_7 0x3
#define ATAPIO_PORT_OFFSET_LBA_8_15 0x4
#define ATAPIO_PORT_OFFSET_LBA_16_23 0x5
#define ATAPIO_PORT_OFFSET_HEAD 0x6
#define ATAPIO_PORT_OFFSET_COMMAND 0x7
#define ATAPIO_PORT_OFFSET_REGULARSTATUS 0x7
#define ATAPIO_PORT_OFFSET_REGULARSTATUS2 0x206

#define ATAPIO_STATUSFLAG_ERR 0x1
#define ATAPIO_STATUSFLAG_DRQ 0x8
#define ATAPIO_STATUSFLAG_SRV 0x10
#define ATAPIO_STATUSFLAG_DF 0x20
#define ATAPIO_STATUSFLAG_RDY 0x40
#define ATAPIO_STATUSFLAG_BSY 0x80

#define ATAPIO_COMMAND_READ 0x20
#define ATAPIO_COMMAND_WRITE 0x30
#define ATAPIO_COMMAND_CACHEFLUSH 0xE7

#define SECTOR_SIZE 512

int ATAPIO_PollStatus(){
	uint8 status;
	while(true){
		status = inb(ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_REGULARSTATUS);
		if( (status & ATAPIO_STATUSFLAG_BSY)==0 && (status & ATAPIO_STATUSFLAG_DRQ)!=0 )
			return ATAPIO_DONE;
		if( (status & ATAPIO_STATUSFLAG_ERR) )
			return ATAPIO_ERROR;
		if( (status & ATAPIO_STATUSFLAG_DF) )
			return ATAPIO_FAULT;
	}
}

static inline void SelectDiskAndSector( drive Drive,uint32 addr){
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_HEAD, 0xE0 | ((uint8)Drive<<4) | ((addr>>24)&0xf) );
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_LBA_0_7, (uint8)addr);
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_LBA_8_15, (uint8)(addr>>8));
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_LBA_16_23, (uint8)(addr>>16));
}
static inline void SendReadCommand(){
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_COMMAND, ATAPIO_COMMAND_READ);
}
static inline void SendWriteCommand(){
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_COMMAND, ATAPIO_COMMAND_WRITE);
}
static inline void PrepareAta(){
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_FEATURES, NULL );
}
static inline void SetCountOfUsebleSectors(uint8 count){
	outb( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_SECTORCOUNT, count );
}

//Чтение из секттора, выбранного SelectDiskAndSector
static inline void ReadSector( uint16* dst ){
	repInsw( ATAPIO_BASE_ADDR + ATAPIO_PORT_OFFSET_DATA, dst, 0x100); 
}
// Запись в сектор, выбранный SelectDiskAndSector
static inline void WriteSector( uint16* src ){
	asm volatile(
		"ATA_loop_WriteSector: \n" 
		"dec %%ecx\n" 
		"outsw\n" 
		"testl %%ecx,%%ecx\n" 
		"jnz ATA_loop_WriteSector\n" 
		:: "Nd"(ATAPIO_BASE_ADDR+ATAPIO_PORT_OFFSET_DATA),"S"(src), "c"(0x100)
	);
}


int ATAPIO_Read( uint32 addr, uint8 count, drive Drive, void * dst){
	PrepareAta();
	SetCountOfUsebleSectors(count);
	SelectDiskAndSector( Drive,addr );
	SendReadCommand();
	int status, i;
	for(i=0;i<count;++i){
		status=ATAPIO_PollStatus();
		if( status!=ATAPIO_DONE )
			return status;
		ReadSector( ( (uint16*)dst + (SECTOR_SIZE/sizeof(uint16))*i  ) );
	}
	return ATAPIO_DONE;
}

int ATAPIO_Write( uint32 addr, uint8 count, drive Drive, const void * src){
	PrepareAta();
	SetCountOfUsebleSectors(count);
	SelectDiskAndSector( Drive,addr );
	SendWriteCommand( addr );
	int status, i;
	for( i=0; i<count; ++i){
		status=ATAPIO_PollStatus();
		if( status!=ATAPIO_DONE )
			return status;
		WriteSector( (uint16*)src + (SECTOR_SIZE/sizeof(uint16))*i );
	}
	return ATAPIO_DONE;
}
