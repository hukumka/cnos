#include <keyboard.h>

#define BUFFER_SIZE 256
#define KEYBOARD_STATUS_ARRAY_SIZE 256

#define DATA_PORT 0x60
#define CONTROL_PORT 0x64

#define STATUS_PS2_BUFFER_NO_EMPTY 0x1D

#define SCANCODE_PREFIX_SPECIAL1 0xE0
#define SCANCODE_PREFIX_SPECIAL2 0xE1
#define SCANCODE_PRIFIX_REALEASE 0x80

#define NO_PRESSED_KEY 0
#define NO_SUITABLE_SCAN_CODE 0
#define PRINT_SCREEN_PRESSED 0xE037E02A
#define PRINT_SCREEN_REALEASED 0xE0AAE0B7
#define PAUSE_PRESSED  ((int)0x1D45E19DC5)

static int keyboardBuffer_pointer=0;
static int keyboardBuffer[BUFFER_SIZE];
//static bool keyboardState[KEYBOARD_STATUS_ARRAY_SIZE];

static int GetScanCode();
static int GetScanCodeSpecial1();
int GetScanCodeSpecial2();

static char GetCharFromScanCode(int scanCode);

static void OnKeyDown(int keyCode);
static void OnKeyUp(int keyCode);


void ParseKeyboardInput(){
	while( inb(CONTROL_PORT) == STATUS_PS2_BUFFER_NO_EMPTY ){
		int keyCode = GetCharFromScanCode(GetScanCode());
		if( keyCode != NO_PRESSED_KEY ){
			keyboardBuffer[keyboardBuffer_pointer++] = keyCode;
		}
	}
}

static int GetScanCode(){
	int scan = inb(DATA_PORT);
	if( scan == SCANCODE_PREFIX_SPECIAL1 ){
		return GetScanCodeSpecial1();
	}else if( scan == SCANCODE_PREFIX_SPECIAL2){
		return GetScanCodeSpecial2();
	}else if( scan < SCANCODE_PREFIX_SPECIAL1  ){
		if( scan < SCANCODE_PRIFIX_REALEASE ){
			OnKeyDown( scan );
			return scan;
		}else{
			OnKeyUp( scan );
			return NO_SUITABLE_SCAN_CODE;
		}
	}else{
		return NO_SUITABLE_SCAN_CODE;
	}
}

static int GetScanCodeSpecial1(){
	int keyState = (SCANCODE_PREFIX_SPECIAL1<<8) | inb( DATA_PORT );
	if( (keyState&0xFF) > SCANCODE_PRIFIX_REALEASE ){
		OnKeyUp( keyState - SCANCODE_PREFIX_SPECIAL1 );
	}else{
		OnKeyDown( keyState );
	}
	return keyState;
}

int GetScanCodeSpecial2(){
	int keyState = 0;
	for(int i=0;i<5;++i){
		keyState = (keyState<<8) | inb(DATA_PORT);
	}
	if( keyState == PAUSE_PRESSED ){
		OnKeyDown( KEY_PAUSE );
	}
	return NO_SUITABLE_SCAN_CODE;
}

const char CharMap[256] = {
//  	0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		0,		0,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	0,		// 0
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',	'O',	'P',	'[',	']',	'\n',	0,		'A',	'S',	// 1
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		'D',	'F',	'G',	'H',	'J',	'K',	'L',	';',	'\'',	'`',	0,		'\\',	'Z',	'X',	'C',	'V',	// 2
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		'B',	'N',	'M',	',',	'.',	'/',	0,		'*',	0,		' ',	0,		0,		0,		0,		0,		0,		// 3
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		0,		0,		0,		0,		0,		0,		0,		'7',	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',	// 4
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		'2',	'3',	'0',	'.',	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 5
//	---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---		---
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 6
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 7
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 8
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 9
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// A
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// B
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// C
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// D
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// E
		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0		// F
};

static char GetCharFromScanCode(int scanCode){
	if( scanCode == NO_SUITABLE_SCAN_CODE ){
		return NO_PRESSED_KEY;
	}else if( (scanCode>>8) == SCANCODE_PREFIX_SPECIAL1 ){
		return CharMap[scanCode&0xff];
	}else{
		return CharMap[scanCode];
	}
}

static void OnKeyDown(int keyCode){

}
static void OnKeyUp(int keyCode){

}
