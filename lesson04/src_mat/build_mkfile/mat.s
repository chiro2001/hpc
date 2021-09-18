	.file	"mat.c"
	.text
	.section	.rodata
.LC0:
	.string	"src/mat.c"
.LC1:
	.string	"mat"
	.text
	.globl	mat_create
	.type	mat_create, @function
mat_create:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movq	$0, -8(%rbp)
	movl	$24, %edi
	call	malloc@PLT
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L2
	leaq	__PRETTY_FUNCTION__.3160(%rip), %rcx
	movl	$24, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	__assert_fail@PLT
.L2:
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %edx
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	-24(%rbp), %edx
	movl	%edx, 4(%rax)
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	mat_create, .-mat_create
	.section	.rodata
.LC2:
	.string	"mat->content"
.LC3:
	.string	"mat->data"
	.text
	.globl	mat_data_init
	.type	mat_data_init, @function
mat_data_init:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L5
	movl	$0, %eax
	jmp	.L6
.L5:
	movq	-24(%rbp), %rax
	movl	(%rax), %edx
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	imull	%edx, %eax
	cltq
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 16(%rax)
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	testq	%rax, %rax
	jne	.L7
	leaq	__PRETTY_FUNCTION__.3164(%rip), %rcx
	movl	$40, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC2(%rip), %rdi
	call	__assert_fail@PLT
.L7:
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	cltq
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	jne	.L8
	leaq	__PRETTY_FUNCTION__.3164(%rip), %rcx
	movl	$44, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC3(%rip), %rdi
	call	__assert_fail@PLT
.L8:
	movl	$0, -4(%rbp)
	jmp	.L9
.L10:
	movq	-24(%rbp), %rax
	movq	16(%rax), %rdx
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	imull	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rsi
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	addq	%rsi, %rdx
	movq	%rdx, (%rax)
	addl	$1, -4(%rbp)
.L9:
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L10
	movq	-24(%rbp), %rax
.L6:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	mat_data_init, .-mat_data_init
	.globl	mat_data_init_fast
	.type	mat_data_init_fast, @function
mat_data_init_fast:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cltq
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	jne	.L12
	leaq	__PRETTY_FUNCTION__.3172(%rip), %rcx
	movl	$66, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC3(%rip), %rdi
	call	__assert_fail@PLT
.L12:
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	mat_data_init_fast, .-mat_data_init_fast
	.globl	mat_free
	.type	mat_free, @function
mat_free:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L15
.L16:
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	free@PLT
	addl	$1, -4(%rbp)
.L15:
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L16
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free@PLT
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	mat_free, .-mat_free
	.globl	mat_free_fast
	.type	mat_free_fast, @function
mat_free_fast:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	mat_free_fast, .-mat_free_fast
	.globl	mat_clone
	.type	mat_clone, @function
mat_clone:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movl	4(%rax), %edx
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init
	movl	$0, -16(%rbp)
	jmp	.L19
.L22:
	movl	$0, -12(%rbp)
	jmp	.L20
.L21:
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-16(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movl	-16(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movq	(%rax), %rax
	movl	-12(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movsd	(%rdx), %xmm0
	movsd	%xmm0, (%rax)
	addl	$1, -12(%rbp)
.L20:
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, -12(%rbp)
	jl	.L21
	addl	$1, -16(%rbp)
.L19:
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	cmpl	%eax, -16(%rbp)
	jl	.L22
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	mat_clone, .-mat_clone
	.globl	mat_crop
	.type	mat_crop, @function
mat_crop:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movl	%esi, -44(%rbp)
	movl	%edx, -48(%rbp)
	movl	%ecx, -52(%rbp)
	movl	%r8d, -56(%rbp)
	movl	-44(%rbp), %eax
	cmpl	-52(%rbp), %eax
	jg	.L25
	movl	-48(%rbp), %eax
	cmpl	-56(%rbp), %eax
	jg	.L25
	cmpq	$0, -40(%rbp)
	jne	.L26
.L25:
	movl	$0, %eax
	jmp	.L27
.L26:
	movl	-52(%rbp), %eax
	subl	-44(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-56(%rbp), %eax
	subl	-48(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init
	movl	$0, -24(%rbp)
	jmp	.L28
.L31:
	movl	$0, -20(%rbp)
	jmp	.L29
.L30:
	movq	-40(%rbp), %rax
	movq	8(%rax), %rax
	movl	-24(%rbp), %ecx
	movl	-44(%rbp), %edx
	addl	%ecx, %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-20(%rbp), %ecx
	movl	-48(%rbp), %edx
	addl	%ecx, %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movl	-24(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movq	(%rax), %rax
	movl	-20(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movsd	(%rdx), %xmm0
	movsd	%xmm0, (%rax)
	addl	$1, -20(%rbp)
.L29:
	movl	-20(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jl	.L30
	addl	$1, -24(%rbp)
.L28:
	movl	-24(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jl	.L31
	movq	-8(%rbp), %rax
.L27:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	mat_crop, .-mat_crop
	.globl	mat_padding_around
	.type	mat_padding_around, @function
mat_padding_around:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movl	%esi, -44(%rbp)
	cmpq	$0, -40(%rbp)
	jne	.L33
	movl	$0, %eax
	jmp	.L34
.L33:
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	movl	-44(%rbp), %edx
	addl	%edx, %edx
	addl	%edx, %eax
	movl	%eax, -16(%rbp)
	movq	-40(%rbp), %rax
	movl	4(%rax), %eax
	movl	-44(%rbp), %edx
	addl	%edx, %edx
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init
	movl	$0, -24(%rbp)
	jmp	.L35
.L38:
	movl	$0, -20(%rbp)
	jmp	.L36
.L37:
	movq	-40(%rbp), %rax
	movq	8(%rax), %rax
	movl	-24(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movl	-24(%rbp), %esi
	movl	-44(%rbp), %ecx
	addl	%esi, %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movq	(%rax), %rax
	movl	-20(%rbp), %esi
	movl	-44(%rbp), %ecx
	addl	%esi, %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movsd	(%rdx), %xmm0
	movsd	%xmm0, (%rax)
	addl	$1, -20(%rbp)
.L36:
	movq	-40(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, -20(%rbp)
	jl	.L37
	addl	$1, -24(%rbp)
.L35:
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	cmpl	%eax, -24(%rbp)
	jl	.L38
	movq	-8(%rbp), %rax
.L34:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	mat_padding_around, .-mat_padding_around
	.section	.rodata
.LC5:
	.string	"#ERR"
	.text
	.globl	mat_kernel_calc
	.type	mat_kernel_calc, @function
mat_kernel_calc:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	pxor	%xmm0, %xmm0
	movsd	%xmm0, -8(%rbp)
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$3, %eax
	jne	.L40
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	$3, %eax
	je	.L41
.L40:
	leaq	.LC5(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movsd	-8(%rbp), %xmm0
	jmp	.L42
.L41:
	movl	$0, -16(%rbp)
	jmp	.L43
.L46:
	movl	$0, -12(%rbp)
	jmp	.L44
.L45:
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	-32(%rbp), %rax
	addq	%rax, %rdx
	movl	-12(%rbp), %eax
	cltq
	movsd	(%rdx,%rax,8), %xmm1
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movl	-16(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movsd	(%rax), %xmm0
	mulsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movzbl	%al, %eax
	cvtsi2sdl	%eax, %xmm0
	movsd	-8(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	addl	$1, -12(%rbp)
.L44:
	cmpl	$2, -12(%rbp)
	jle	.L45
	addl	$1, -16(%rbp)
.L43:
	cmpl	$2, -16(%rbp)
	jle	.L46
	movsd	-8(%rbp), %xmm0
.L42:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	mat_kernel_calc, .-mat_kernel_calc
	.globl	mat_conv
	.type	mat_conv, @function
mat_conv:
.LFB15:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	-56(%rbp), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	mat_padding_around
	movq	%rax, -40(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	mat_clone
	movq	%rax, -32(%rbp)
	movl	$0, -48(%rbp)
	jmp	.L48
.L51:
	movl	$0, -44(%rbp)
	jmp	.L49
.L50:
	movl	-44(%rbp), %eax
	leal	3(%rax), %edi
	movl	-48(%rbp), %eax
	leal	3(%rax), %ecx
	movl	-44(%rbp), %edx
	movl	-48(%rbp), %esi
	movq	-40(%rbp), %rax
	movl	%edi, %r8d
	movq	%rax, %rdi
	call	mat_crop
	movq	%rax, -24(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	movl	-48(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-44(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	movq	-64(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	mat_kernel_calc
	movq	%xmm0, %rax
	movq	%rax, (%rbx)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	mat_free
	addl	$1, -44(%rbp)
.L49:
	movq	-56(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, -44(%rbp)
	jl	.L50
	addl	$1, -48(%rbp)
.L48:
	movq	-56(%rbp), %rax
	movl	(%rax), %eax
	cmpl	%eax, -48(%rbp)
	jl	.L51
	movq	-32(%rbp), %rax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	mat_conv, .-mat_conv
	.globl	mat_mul
	.type	mat_mul, @function
mat_mul:
.LFB16:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %rax
	movl	(%rax), %edx
	movq	-48(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, %edx
	je	.L54
	movl	$0, %eax
	jmp	.L55
.L54:
	movq	$0, -16(%rbp)
	cmpq	$0, -56(%rbp)
	je	.L56
	movq	-56(%rbp), %rax
	movq	%rax, -16(%rbp)
	jmp	.L57
.L56:
	movq	-48(%rbp), %rax
	movl	(%rax), %edx
	movq	-40(%rbp), %rax
	movl	4(%rax), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	mat_create
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	mat_data_init
.L57:
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	movl	$0, -32(%rbp)
	jmp	.L58
.L63:
	movl	$0, -28(%rbp)
	jmp	.L59
.L62:
	pxor	%xmm0, %xmm0
	movsd	%xmm0, -8(%rbp)
	movl	$0, -24(%rbp)
	jmp	.L60
.L61:
	movq	-40(%rbp), %rax
	movq	8(%rax), %rax
	movl	-32(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-24(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movsd	(%rax), %xmm1
	movq	-48(%rbp), %rax
	movq	8(%rax), %rax
	movl	-24(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-28(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movsd	(%rax), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	-8(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	addl	$1, -24(%rbp)
.L60:
	movl	-24(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L61
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movl	-32(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-28(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movsd	-8(%rbp), %xmm0
	movsd	%xmm0, (%rax)
	addl	$1, -28(%rbp)
.L59:
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	cmpl	%eax, -28(%rbp)
	jl	.L62
	addl	$1, -32(%rbp)
.L58:
	movq	-16(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, -32(%rbp)
	jl	.L63
	movq	-16(%rbp), %rax
.L55:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	mat_mul, .-mat_mul
	.section	.rodata
	.align 8
	.type	__PRETTY_FUNCTION__.3160, @object
	.size	__PRETTY_FUNCTION__.3160, 11
__PRETTY_FUNCTION__.3160:
	.string	"mat_create"
	.align 8
	.type	__PRETTY_FUNCTION__.3164, @object
	.size	__PRETTY_FUNCTION__.3164, 14
__PRETTY_FUNCTION__.3164:
	.string	"mat_data_init"
	.align 16
	.type	__PRETTY_FUNCTION__.3172, @object
	.size	__PRETTY_FUNCTION__.3172, 19
__PRETTY_FUNCTION__.3172:
	.string	"mat_data_init_fast"
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
