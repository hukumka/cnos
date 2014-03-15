#include <types.h>
#include <io.h>
#include <ata.h>
#include <pci.h>

uint16 *VIDEODATA = (uint16*) 0xB8000;
int main(){
	int i;
	VIDEODATA[0]=VIDEODATA[1]=1;
	for(i=2;i<256;++i){
		VIDEODATA[i]=VIDEODATA[i-1]+VIDEODATA[i-2];
	}
//	ATAPIO_Write(0,1,master,VIDEODATA);
	return 0;
}
