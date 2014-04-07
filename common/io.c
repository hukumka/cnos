#include <io.h>

/////////////
// ВыВОД
/////////////

uint8 X=0,Y=0;
uint8 CurrentColor=0x07; // Светло серый на черном
uint8 tabstop=0x4;
		
void ClearScreen(){
	for(int i=0;i<SCREEN_WIDTH*SCREEN_HEIGHT;++i)
		VIDEOBUFFER[i]=0;
	X=Y=0;
}

void putchar( char a){
	switch( a ){
		case '\t':
			do{
				PUTC_POS(' ',CurrentColor,X,Y);
				++X;
			}while( X%tabstop );
			break;
		case '\b':
			if(X>0){
				--X;
				PUTC_POS(' ',CurrentColor,X-1,Y);
			}
			return;
		case '\n':
			++Y;
		case '\r':
			X=0;
			break;
		default:
			PUTC_POS(a,CurrentColor,X,Y);
			++X;
	}
	if( X>=SCREEN_WIDTH ){
		++Y;
		X=0;
	}
	if( Y>=SCREEN_HEIGHT )
		ClearScreen();
}

void printf_d(int d){
	char str[10];
	int i=0;
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
	char *p = (char*)&format+sizeof(char*);
	for(int i=0; format[i]; ++i){
		if( format[i]=='%' ){
			i++;
			switch(format[i]){
				case 0:
					return count;
				case '%':
					putchar('%');
					break;
				case 'd':
					printf_d( *(int*)p);
					p+=sizeof(int);
					count++;
					break;
				case 's':
					printf_s( *(char**)(p) );
					p+=sizeof(char*);
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

int getchar(){
	return 0;	
}
