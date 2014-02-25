#pragma once

#include <types.h>

#define PCI_PORT_ADDR 0xcf8
#define PCI_PORT_DATA 0xcfc

#define PCI_MAX_BUSES 0xff
#define PCI_MAX_DEVICES 0x20
#define PCI_MAX_FUNCTIONS 0x8

#define PCI_HEADERTYPE_NORMAL 0x0
#define PCI_HEADERTYPE_BRIGE 0x1
#define PCI_HEADERTYPE_CARDBUS 0x2
#define PCI_HEADERTYPE_MULTIFUNC 0x80

#define PCI_MAX_SAME_DEVICES 0x10

typedef struct {
	uint8 bus:8;
	uint8 dev:4;
	uint8 func:4;
}PCI_Uaddr;

typedef union{
	struct{
		uint16 vendorId;
		uint16 deviceId;
		uint16 command;
		uint16 status;
		uint8 recognizeId;
		uint8 progIF;
		uint8 subclass;
		uint8 class;
		uint8 cls;
		uint8 latencyTimer;
		uint8 headerType;
		uint8 BIST;
	};
	uint32 value[4];
}PCI_Header;

uint32 PCI_ReadUInt32( PCI_Uaddr addr, uint8 reg);
bool PCI_ReadHeader( PCI_Uaddr addr, PCI_Header *header);
void PCI_Scan();
