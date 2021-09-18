	.file	"mat_mul_test.c"
	.text
	.section	.rodata
.LC0:
	.string	"\344\275\277\347\224\250\351\273\230\350\256\244: K = %d\n"
.LC1:
	.string	"\344\275\277\347\224\250: K = %d\n"
.LC2:
	.string	"\345\210\235\345\247\213\345\214\226\344\270\244\344\270\252\347\237\251\351\230\265"
.LC3:
	.string	"\345\210\235\345\247\213\345\214\226 C"
	.align 8
.LC4:
	.string	"\347\256\200\345\215\225\347\237\251\351\230\265\350\256\241\347\256\227: \345\274\200\345\247\213\350\256\241\346\227\266"
	.align 8
.LC6:
	.string	"\347\256\200\345\215\225\347\237\251\351\230\265: \350\256\241\347\256\227\347\224\250\346\227\266: %lfs\n"
.LC7:
	.string	"OpenBLAS: \345\274\200\345\247\213\350\256\241\346\227\266"
.LC10:
	.string	"OpenBLAS: \350\256\241\347\256\227\347\224\250\346\227\266: %lfs\n"
.LC11:
	.string	"w"
.LC12:
	.string	"results.txt"
.LC13:
	.string	"Cannot open results.txt!!\n"
.LC14:
	.string	"%lf\n%lf\n"
.LC15:
	.string	"DONE."
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	addq	$-128, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movl	%edi, -132(%rbp)
	movq	%rsi, -144(%rbp)
	movl	$101, -112(%rbp)
	movl	$111, -108(%rbp)
	movl	$111, -104(%rbp)
	movl	$128, -124(%rbp)
	cmpl	$1, -132(%rbp)
	jg	.L2
	movl	-124(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L3
.L2:
	movq	-144(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -124(%rbp)
	movl	-124(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L3:
	movl	-124(%rbp), %eax
	movl	%eax, -100(%rbp)
	movl	-124(%rbp), %eax
	movl	%eax, -96(%rbp)
	movl	-124(%rbp), %eax
	movl	%eax, -92(%rbp)
	movl	-96(%rbp), %eax
	movl	%eax, -88(%rbp)
	movl	-96(%rbp), %eax
	movl	%eax, -84(%rbp)
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	movl	-96(%rbp), %edx
	movl	-100(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create@PLT
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init@PLT
	movl	-100(%rbp), %edx
	movl	-96(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create@PLT
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init@PLT
	movl	$0, -120(%rbp)
	jmp	.L4
.L7:
	movl	$0, -116(%rbp)
	jmp	.L5
.L6:
	movl	-120(%rbp), %edx
	movl	-116(%rbp), %eax
	addl	%edx, %eax
	cvtsi2sdl	%eax, %xmm0
	movq	-80(%rbp), %rax
	movq	8(%rax), %rax
	movl	-120(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-116(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	call	sin@PLT
	movq	%xmm0, %rax
	movq	%rax, (%rbx)
	movl	-116(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-120(%rbp), %eax
	addl	%edx, %eax
	cvtsi2sdl	%eax, %xmm0
	movq	-72(%rbp), %rax
	movq	8(%rax), %rax
	movl	-120(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-116(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	call	cos@PLT
	movq	%xmm0, %rax
	movq	%rax, (%rbx)
	addl	$1, -116(%rbp)
.L5:
	movl	-116(%rbp), %eax
	cmpl	-96(%rbp), %eax
	jl	.L6
	addl	$1, -120(%rbp)
.L4:
	movl	-120(%rbp), %eax
	cmpl	-100(%rbp), %eax
	jl	.L7
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	movl	-100(%rbp), %edx
	movl	-100(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create@PLT
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init@PLT
	leaq	.LC4(%rip), %rdi
	call	puts@PLT
	call	clock@PLT
	movq	%rax, -56(%rbp)
	movq	-64(%rbp), %rdx
	movq	-72(%rbp), %rcx
	movq	-80(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	mat_mul@PLT
	call	clock@PLT
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	subq	-56(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC6(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	leaq	.LC7(%rip), %rdi
	call	puts@PLT
	call	clock@PLT
	movq	%rax, -56(%rbp)
	movq	-64(%rbp), %rax
	movq	16(%rax), %r10
	movq	-72(%rbp), %rax
	movq	16(%rax), %r9
	movq	-80(%rbp), %rax
	movq	16(%rax), %r8
	movq	.LC8(%rip), %rdi
	movl	-124(%rbp), %r12d
	movl	-96(%rbp), %ebx
	movl	-100(%rbp), %ecx
	movl	-104(%rbp), %edx
	movl	-108(%rbp), %esi
	movl	-112(%rbp), %eax
	movl	-84(%rbp), %r11d
	pushq	%r11
	pushq	%r10
	movl	-88(%rbp), %r10d
	pushq	%r10
	pushq	%r9
	movl	-92(%rbp), %r9d
	pushq	%r9
	pushq	%r8
	pxor	%xmm1, %xmm1
	movq	%rdi, %xmm0
	movl	%r12d, %r9d
	movl	%ebx, %r8d
	movl	%eax, %edi
	call	cblas_dgemm@PLT
	addq	$48, %rsp
	call	clock@PLT
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	subq	-56(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC10(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	leaq	.LC11(%rip), %rsi
	leaq	.LC12(%rip), %rdi
	call	fopen@PLT
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L8
	leaq	.LC13(%rip), %rdi
	call	perror@PLT
	movl	$1, %eax
	jmp	.L9
.L8:
	movsd	-32(%rbp), %xmm0
	movq	-40(%rbp), %rdx
	movq	-24(%rbp), %rax
	movapd	%xmm0, %xmm1
	movq	%rdx, %xmm0
	leaq	.LC14(%rip), %rsi
	movq	%rax, %rdi
	movl	$2, %eax
	call	fprintf@PLT
	leaq	.LC15(%rip), %rdi
	call	puts@PLT
	movl	$0, %eax
.L9:
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC5:
	.long	0
	.long	1093567616
	.align 8
.LC8:
	.long	0
	.long	1072693248
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
