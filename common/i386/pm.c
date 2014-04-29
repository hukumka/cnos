#include <pm.h>
#include <memory.h>
#include <int.h>
#include <macros.h>
#include <pic.h>

// GDTR 
#define USER_SEGMENTS_COUNT 100

static void SetupGDT(uint32 newAddr);
static void ApplyGDT();
static void AddDescriptor(uint32 base, uint32 limit, uint16 flag);
static void SetDescriptor( int id, uint32 base, uint32 limit, uint16 flag);

struct __attribute__ ((__packed__)){
	uint16 size;
	uint32* gdt_pointer;
} GDTR;
int UserSegmentsBase;

void InitNewGDT(uint32 newAddr){
	SetupGDT(newAddr);
	AddDescriptor(0,0xf0000, GDT_CODE_PL0);
	AddDescriptor(0,0xf0000, GDT_DATA_PL0);

	UserSegmentsBase = GDTR.size;
	// Занмаем место для дескрипторов, которые могут понадобиться в будущем
	for(int i=0;i<USER_SEGMENTS_COUNT;++i)
		AddDescriptor(0,0,0);
	ApplyGDT();
}

static void SetupGDT(uint32 newAddr){
	GDTR.gdt_pointer = (void*)newAddr;
	GDTR.size = 0;
	AddDescriptor(0,0,0);
} 

static void ApplyGDT(){
	GDTR.size = GDTR.size * 8 -1;
	asm volatile("lgdt (%0)\n"::"r"(&GDTR) ); 
	GDTR.size = (GDTR.size+1) /8;
} 

static void AddDescriptor(uint32 base, uint32 limit, uint16 flag){
	SetDescriptor(GDTR.size++, base, limit, flag);
} 

static void SetDescriptor( int id, uint32 base, uint32 limit, uint16 flag){
    // Create the high 32 bit segment
    (GDTR.gdt_pointer)[id*2+1]  =  limit       & 0x000F0000;         // set limit bits 19:16
    (GDTR.gdt_pointer)[id*2+1] |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    (GDTR.gdt_pointer)[id*2+1] |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    (GDTR.gdt_pointer)[id*2+1] |=  base        & 0xFF000000;         // set base bits 31:24
    // Create the low 32 bit segment
    (GDTR.gdt_pointer)[id*2+0] = base  << 16;                       // set base bits 15:0
    (GDTR.gdt_pointer)[id*2+0] |= limit  & 0x0000FFFF;               // set limit bits 15:0
} 


static inline bool IsDescriptorFree( int id );
static inline int GetFreeUserDescriptorId();
static int NewSuperUserDescriptor( uint32 baseAddress, uint32 granularySize, int type );

int NewSuperUserDataDescriptor( uint32 baseAddress, uint32  grSize){
	return NewSuperUserDescriptor( baseAddress, grSize, DATA );
}

int NewSuperUserCodeDescriptor( uint32 baseAddress, uint32  grSize){
	return NewSuperUserDescriptor( baseAddress, grSize, CODE );
}

static int NewSuperUserDescriptor( uint32 baseAddress, uint32 granularySize, int type ){
	int freeDescriptorId = GetFreeUserDescriptorId();
	if( ! freeDescriptorId ){
		return 0;
	}
	int flag = (type==CODE)? (GDT_CODE_PL0) : (GDT_DATA_PL0);
	SetDescriptor( freeDescriptorId, baseAddress, granularySize, flag );
	return freeDescriptorId<<3 | (0<<2) | 0 ;
}

static inline int GetFreeUserDescriptorId(){
	for( int i=0; i<USER_SEGMENTS_COUNT;++i ){
		if( IsDescriptorFree(UserSegmentsBase+i) ){
			return i+UserSegmentsBase;
		}
	}
	return 0;
}

static inline bool IsDescriptorFree( int id ){
	return (*(GDTR.gdt_pointer+id*2) == 0) && (*(GDTR.gdt_pointer+id*2+1) == 0);
} 


// IDTR

struct __attribute__ ((__packed__)){
	uint16 size;
	uint32 *idt;
} IDTR;

#define INTERRUPT_TASK_GATE(PL) (0xE | (1<<7) | (PL&3)<<5 )

static void SetupIDT( uint32 newAddr );
static void ApplyIDT();
static void FillInterrupts( uint16 count );
static void SetInterruptDescriptor( uint16 id, uint16 selector, uint32 addr, uint8 type );

void InitNewIDT( uint32 newAddr ){
	PIC_remap(0x20,0x28);
	SetupIDT( newAddr );
	FillInterrupts( 256 );
	
	// keyboard
	SetInterruptDescriptor( 0x21, 0x8, (uint32)Keyboard_int, INTERRUPT_TASK_GATE(0) );
	
	
	ApplyIDT();
}

static void SetupIDT( uint32 newAddr ){
	IDTR.size = 0;
	IDTR.idt = (uint32*)newAddr;
}

static void ApplyIDT(){
	IDTR.size = IDTR.size*8-1;
	asm("lidt (%0)\n"::"r"(&IDTR));
	IDTR.size = (IDTR.size+1) /8;
}

static void FillInterrupts( uint16 count ){
	for( int i=0; i<2*count; ++i ){
		*(IDTR.idt+i) = 0;
	}
	IDTR.size = count;
}

static void SetInterruptDescriptor( uint16 id, uint16 selector, uint32 addr, uint8 type ){
	*(IDTR.idt + id*2) = (addr&0xFFFF) | ((uint32)selector<<16);
	*(IDTR.idt + id*2 + 1) = (addr&0xFFFF0000) | ((uint32)type<<8);
}

