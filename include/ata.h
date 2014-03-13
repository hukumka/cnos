/*
ATA сервис

Функционал:

	int ATAPIO_Read( uint32 start, uint8 count, drive Drive, void * dst );

	Читает с диска в область памяти dst count секторов, начиная с start
		start - 28 бит на номер сектора, с которого начнется чтение. Старшие 4 бита отбрасываются.
		count - количество секторов для чтения за раз. 
		Drive - диск, с которого происходит чтение. Только master/slave.
		dst - указатель на область памяти, в которую будет записано то, что будет прочтено с диска
	Возвращаемое значение - индикатор успешости работы.
		ATAPIO_DONE - все прошло на ура
		ATAPIO_ERROR - ошибка при обработке команды
		ATAPIO_FAULT - диск спекся


	int ATAPIO_Write( uint32 start, uint8 count, drive Drive, const void * src );
	
	Пишет на диск count секторов из области памяти dst, начиная с сектора start
		start - 28 бит на номер сектора с которого начинается запись. Старшие 4 бита отбрасываются
		count - количество секторов для записи
		Drive - диск, с которого происходит запись. Только master/slave.
		dst - указатель на область памяти, из которой берется информация для записи
	Возвращаемое значение - индикатор успешости работы.
		ATAPIO_DONE - все прошло на ура
		ATAPIO_ERROR - ошибка при обработке команды
		ATAPIO_FAULT - диск спекся

*/

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
int ATAPIO_Read( uint32 start, uint8 count, drive Drive, void * dst );
int ATAPIO_Write( uint32 start, uint8 count, drive Drive, const void * src1);
