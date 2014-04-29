#include <pic.h>

// PIC

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define ICW1_ICW4 0x1
#define ICW1_SINGLE 0x2
#define ICW1_INTERVAL4 0x4
#define ICW1_LEVEL 0x8
#define ICW1_INIT 0x10

#define ICW4_8086 0x1
#define ICW4_AUTO 0x2
#define ICW4_BUF_SLAVE 0x8
#define ICW4_BUF_MASTER 0xC
#define ICW4_SFNM 0x10


void PIC_remap( int offset1, int offset2 ){
	uint8 mask1,mask2;
	mask1 = inb(PIC_MASTER_DATA);
	mask2 = inb(PIC_SLAVE_DATA);

	outb(PIC_MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC_SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC_MASTER_DATA,offset1);
	io_wait();
	outb(PIC_SLAVE_COMMAND, offset2);
	io_wait();
	outb(PIC_MASTER_DATA, 4);
	io_wait();
	outb(PIC_SLAVE_DATA, 2);
	io_wait();

	outb(PIC_MASTER_DATA,mask1);
	outb(PIC_SLAVE_DATA,mask2);
}

void IRQ_set_mask( uint8 mask){
	outb( PIC_MASTER_DATA, mask );
}
