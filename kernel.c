#include <types.h>
#include <io.h>
#include <ata.h>
#include <memory.h>
#include <pci.h>
#include <fat32.h>
#include <fs.h>

int main(){
	InitMemory();
	InitFilesystemManager();
	ClearScreen();
	char a[]="qwetsss";
	printf("Here number %d and string \"%s\"", 456,a);
	return 0;
}
