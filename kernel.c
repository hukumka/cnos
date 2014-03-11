#include <types.h>
#include <io.h>
#include <ata.h>
#include <pci.h>

uint16 *VIDEODATA = (uint16*) 0xB8000;
int main(){
	ATAPIO_Read(0,1,master,VIDEODATA);
	return 0;
}
