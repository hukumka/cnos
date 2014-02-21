#include <types.h>
#include <macros.h>

#define VIDEOBUFFER ((uint16*) 0xB8000)

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void putchar(char a); 
int getchar(void);
int printf(const char *format,...);
int scanf(const char *format,...);
