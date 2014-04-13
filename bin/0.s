	.file	"0.c"
	.globl	printf
	.data
	.align 4
	.type	printf, @object
	.size	printf, 4
printf:
	.long	1077444
	.section	.rodata
.LC0:
	.string	"qfdf"
.LC1:
	.string	"qwe %s qwesd %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$16, %esp
	movl	printf, %eax
	movl	$12322, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	$.LC1, (%esp)
	call	*%eax
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
