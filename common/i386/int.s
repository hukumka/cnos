.text

.global Keyboard_int


.set KEYBOARD_BUFFER_SIZE, 0x100

.global keyboard_buffer_size
.global keyboard_buffer_head_pointer
.global keyboard_buffer

keyboard_buffer_head_pointer:
	.short 0
keyboard_buffer:
	.skip KEYBOARD_BUFFER_SIZE

Keyboard_int:
	pusha
		call ParseKeyboardInput;

		movb $0x20, %al
		outb %al, $0x20
	popa
	iret

