#include <types.h>
#include <macros.h>

#define VIDEOBUFFER 0xB8000

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0x2
#define COLOR_CYAN 0x3
#define COLOR_RED 0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN 0x6
#define COLOR_LIGHTGRAY 0x7
#define COLOR_DARKGREY 0x8
#define COLOR_LIGHTBLUE 0x9
#define COLOR_LIGHTGREEN 0xa
#define COLOR_LIGHTCYAN 0xb
#define COLOR_LIGHTRED 0xc
#define COLOR_LIGHTMAGENTA 0xd
#define COLOR_YELLOW 0xe
#define COLOR_WHITE 0xf

#define COLOR_GET(bc, fc) ( (bc<<4)|fc )
/*#define PUTC_POS(ch,cl,x,y) ( VIDEOBUFFER[x+y*SCREEN_WIDTH]=ch|(cl<<8) )*/

void ClearScreen();
void putchar(char a); 
int getchar(void);
int printf(const char *format,...);
int scanf(const char *format,...);
