#include <types.h>

#define ATAPIO_ERROR 0x0
#define ATAPIO_DONE 0x1
#define ATAPIO_FAULT 0x2

typedef enum{
	master,
	slave
} drive;

typedef bool ATA_CHANNEL;
typedef bool ATA_DEVICE_ID;
int ATAPIO_Read( uint32 addr, uint8 count, drive Drive, void * dst );
