#include <types.h>
uint16 *VIDEODATA = (uint16*) 0xB8000;
int  main(){
	for(int i=0;i<80;++i)
		VIDEODATA[i]=0x0f00|(i+30);
}
