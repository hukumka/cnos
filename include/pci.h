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

