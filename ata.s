	.file	"ata.c"
	.text
	.type	inb, @function
inb:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movw	%ax, -20(%rbp)
	movzwl	-20(%rbp), %eax
	movl	%eax, %edx
#APP
# 7 "./include/macros.h" 1
	inb %dx,%al
# 0 "" 2
#NO_APP
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	inb, .-inb
	.type	repInsw, @function
repInsw:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %ecx
	movq	%rsi, -16(%rbp)
	movl	%edx, %eax
	movw	%cx, -4(%rbp)
	movw	%ax, -8(%rbp)
	movzwl	-4(%rbp), %eax
	movq	-16(%rbp), %rsi
	movzwl	-8(%rbp), %ecx
	movl	%eax, %edx
	movq	%rsi, %rdi
#APP
# 21 "./include/macros.h" 1
	rep insw
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	repInsw, .-repInsw
	.type	outb, @function
outb:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %edx
	movl	%esi, %eax
	movw	%dx, -4(%rbp)
	movb	%al, -8(%rbp)
	movzbl	-8(%rbp), %eax
	movzwl	-4(%rbp), %edx
#APP
# 25 "./include/macros.h" 1
	outb %al, %dx
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	outb, .-outb
	.globl	ATAPIO_PollStatus
	.type	ATAPIO_PollStatus, @function
ATAPIO_PollStatus:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
.L10:
	movl	$503, %edi
	call	inb
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	testb	%al, %al
	js	.L6
	movzbl	-1(%rbp), %eax
	andl	$8, %eax
	testl	%eax, %eax
	je	.L6
	movl	$1, %eax
	jmp	.L7
.L6:
	movzbl	-1(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L8
	movl	$0, %eax
	jmp	.L7
.L8:
	movzbl	-1(%rbp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	je	.L9
	movl	$2, %eax
	jmp	.L7
.L9:
	jmp	.L10
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	ATAPIO_PollStatus, .-ATAPIO_PollStatus
	.globl	ATAPIO_Read
	.type	ATAPIO_Read, @function
ATAPIO_Read:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$40, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, %eax
	movl	%edx, -32(%rbp)
	movq	%rcx, -40(%rbp)
	movb	%al, -28(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	movl	-32(%rbp), %eax
	movzbl	%al, %eax
	sall	$4, %eax
	movq	-24(%rbp), %rdx
	shrq	$24, %rdx
	andl	$15, %edx
	orl	%edx, %eax
	orl	$-32, %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$502, %edi
	call	outb
	movl	$0, %esi
	movl	$496, %edi
	call	outb
	movzbl	-28(%rbp), %eax
	movl	%eax, %esi
	movl	$498, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$499, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$500, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$501, %edi
	call	outb
	movl	$32, %esi
	movl	$503, %edi
	call	outb
	movl	$0, -16(%rbp)
	jmp	.L12
.L15:
	movl	$0, %eax
	call	ATAPIO_PollStatus
	movl	%eax, -12(%rbp)
	cmpl	$1, -12(%rbp)
	je	.L13
	movl	-12(%rbp), %eax
	jmp	.L14
.L13:
	movl	-16(%rbp), %eax
	cltq
	salq	$9, %rax
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	$256, %edx
	movq	%rax, %rsi
	movl	$496, %edi
	call	repInsw
	addl	$1, -16(%rbp)
.L12:
	movzbl	-28(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jg	.L15
	movl	$1, %eax
.L14:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	ATAPIO_Read, .-ATAPIO_Read
	.globl	ATAPIO_Write
	.type	ATAPIO_Write, @function
ATAPIO_Write:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$40, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, %eax
	movl	%edx, -32(%rbp)
	movq	%rcx, -40(%rbp)
	movb	%al, -28(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	movl	-32(%rbp), %eax
	movzbl	%al, %eax
	sall	$4, %eax
	movq	-24(%rbp), %rdx
	shrq	$24, %rdx
	andl	$15, %edx
	orl	%edx, %eax
	orl	$-32, %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$502, %edi
	call	outb
	movl	$0, %esi
	movl	$496, %edi
	call	outb
	movzbl	-28(%rbp), %eax
	movl	%eax, %esi
	movl	$498, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$499, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$500, %edi
	call	outb
	movq	-24(%rbp), %rax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$501, %edi
	call	outb
	movl	$48, %esi
	movl	$503, %edi
	call	outb
	movl	$0, -16(%rbp)
	jmp	.L17
.L20:
	movl	$0, %eax
	call	ATAPIO_PollStatus
	movl	%eax, -12(%rbp)
	cmpl	$1, -12(%rbp)
	je	.L18
	movl	-12(%rbp), %eax
	jmp	.L19
.L18:
	movl	-16(%rbp), %eax
	cltq
	salq	$9, %rax
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	leaq	(%rdx,%rax), %rsi
	movl	$496, %eax
	movl	$256, %ecx
	movl	%eax, %edx
#APP
# 74 "common/ata.c" 1
	ATA_loop_WriteSector: dec %ecxoutswtestl %ecx,%ecxjnz ATA_loop_WriteSector
# 0 "" 2
#NO_APP
	addl	$1, -16(%rbp)
.L17:
	movzbl	-28(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jg	.L20
	movl	$1, %eax
.L19:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	ATAPIO_Write, .-ATAPIO_Write
	.ident	"GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
