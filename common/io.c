#include <io.h>
#include <macros.h>

/////////////
// ВыВОД
/////////////

uint8 X=0,Y=0;
uint8 CurrentColor=0x07; // Светло серый на черном
uint8 tabstop=0x4;
	
static void PUTC_POS(char symbol, uint8 color, uint8 x, uint8 y){
	uint32 addr = (x+y*SCREEN_WIDTH)*2 + VIDEOBUFFER;
	uint16 sym = symbol|(color<<8);
	asm("movw %0, %%es:(%1)"::"r"(sym), "r"(addr));
}

void ClearScreen(){
	for(int addr=VIDEOBUFFER;addr<VIDEOBUFFER+SCREEN_WIDTH*SCREEN_HEIGHT;++addr)
		asm("movw $0x0, %%es:(%0)":: "r"(addr));
	X=Y=0;
}

void putchar( char a){
	int8 x,y,color;
	MOVE_ESDATA_TO_LOCAL(X,x);
	MOVE_ESDATA_TO_LOCAL(Y,y);
	MOVE_ESDATA_TO_LOCAL(CurrentColor,color);
	switch( a ){
		case '\t':
			do{
				PUTC_POS(' ',color,x,y);
				++x;
			}while( X%tabstop );
			break;
		case '\b':
			if(x>0){
				--x;
				PUTC_POS(' ',color,x-1,y);
			}
			break;
		case '\n':
			++y;
		case '\r':
			x=0;
			break;
		default:
			PUTC_POS(a,color,x,y);
			++x;
	}
	if( x>=SCREEN_WIDTH ){
		++y;
		y=0;
	}
	if( y>=SCREEN_HEIGHT )
		ClearScreen();
	MOVE_LOCAL_TO_ESDATA(x,X);
	MOVE_LOCAL_TO_ESDATA(y,Y);
}

void printf_d(int d){
	char str[10];
	int i=0;
	if(d==0){
		putchar('0');	
	}
	while(d){
		str[i]=d%10+0x30;
		d/=10;
		++i;
	}
	while(i>0){
		--i;
		putchar(str[i]);
	};
}
void printf_s(const char *s){
	for(int i=0;s[i];++i)
		putchar(s[i]);
}

int printf( const char *format, ...){
	int count=0;
	char *argumenPointer = (char*) &format +sizeof(const char*);
					
	int dvalue;
	char* svalue;
	for(int i=0; format[i]; ++i){
		if( format[i]=='%' ){
			i++;
			switch(format[i]){
				case '\0':
					return count;
				case '%':
					putchar('%');
					break;
				case 'd':
					asm("mov %%ss:(%1), %0":"=r"(dvalue):"r"(argumenPointer));
					printf_d(dvalue);
					argumenPointer+=sizeof(int);
					count++;
					break;
				case 's':
					asm("mov %%ss:(%1), %0":"=r"(svalue):"r"(argumenPointer));
					printf_s( svalue );
					argumenPointer+=sizeof(char*);
					count++;
					break;
				default:
					putchar(format[i]);
			}
		}else{
			putchar(format[i]);
		}
	}
	return count;
}

/////////////
// ВВОД
/////////////

// TODO
int getchar(){
	return 0;
}
