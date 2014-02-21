
.text
.global loader 
.set FLAGS, 0x0
.set MAGIC, 0x1BADB002
.set CHECKSUM, -(MAGIC+FLAGS)

.align 0x4

.long MAGIC
.long FLAGS
.long CHECKSUM

.set STACKSIZE, 0x4000
stack:
	.skip STACKSIZE
	.comm mdb, 4
	.comm magic, 4

loader:
	movl $(stack+STACKSIZE), %esp
	call main
	
	cli
	hlt
	jmp . 
