#pragma once

#define FILE_SEEK_FROMSTART 0
#define FILE_SEEK_FROMEND 1
#define FILE_SEEK_OFFSET 2

void LoadPartitions();
bool SelectDisk( uint8 id );

