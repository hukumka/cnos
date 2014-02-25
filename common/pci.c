#include <macros.h>
#include <pci.h>

typedef union{
	struct{
		uint32 zeros:2;
		uint32 reg:6;
		uint32 func:3;
		uint32 dev:5;
		uint32 bus:8;
		uint32 reversed:7;
		uint32 enable:1;
	};
	uint32 value;
}PCI_addr;

uint32 PCI_ReadUInt32( PCI_Uaddr uaddr, uint8 reg){
	PCI_addr addr;
	addr.value=0; // Забиваем нулями
	addr.reg=reg;
	addr.func=uaddr.func;
	addr.dev=uaddr.dev;
	addr.bus=uaddr.bus;
	addr.enable=1;
	outl( PCI_PORT_ADDR, addr.value);
	return inl( PCI_PORT_DATA );
}

bool PCI_ReadHeader( PCI_Uaddr addr, PCI_Header *header){
	for( uint8 reg=0;reg<4;++reg )
		header->value[reg] = PCI_ReadUInt32( addr, reg );
	if( header->vendorId==0xffff )
		return false;
	return true;
}

PCI_Uaddr RecognizedDevices[256][256][PCI_MAX_SAME_DEVICES];
uint8 RecognizedDevices_Count[256][256];
void PCI_Scan(){
	PCI_Uaddr addr;
	PCI_Header header;
	for( addr.bus=0; addr.bus<PCI_MAX_BUSES; ++addr.bus){
		for( addr.dev=0; addr.dev<PCI_MAX_DEVICES; ++addr.dev){
			addr.func=0;
			if( ! PCI_ReadHeader( addr, &header) )
				continue;
			if( RecognizedDevices_Count[header.class][header.subclass]<PCI_MAX_SAME_DEVICES )
				RecognizedDevices[header.class][header.subclass][RecognizedDevices_Count[header.class][header.subclass]++]=addr;
			if( header.headerType & PCI_HEADERTYPE_MULTIFUNC ){
				for( addr.func=1; addr.func<PCI_MAX_FUNCTIONS; ++addr.func ){
					if( ! PCI_ReadHeader( addr, &header) )
						continue;
					if( RecognizedDevices_Count[header.class][header.subclass]<PCI_MAX_SAME_DEVICES )
						RecognizedDevices[header.class][header.subclass][RecognizedDevices_Count[header.class][header.subclass]++]=addr;
				}
			}
		}
	}
}
