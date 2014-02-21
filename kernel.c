#include <types.h>
uint16 *VIDEODATA = (uint16*) 0xB8000;
void main(){
	VIDEODATA[79]=0x0f30;
	VIDEODATA[39]=0x0f31;
}
