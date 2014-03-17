#include <types.h>
#include <io.h>
#include <ata.h>
#include <memory.h>
#include <pci.h>

uint16 *VIDEODATA = (uint16*) 0xB8000;
int main(){
	InitMemoryTable();
	return 0;
}
