	.file	"decomment.c"
	.section	.rodata
.LC0:
	.string	"decomment.c"
.LC1:
	.string	"0"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -24(%rbp)
	movl	$1, -20(%rbp)
	jmp	.L2
.L13:
	movl	-24(%rbp), %eax
	cmpl	$7, %eax
	ja	.L3
	movl	%eax, %eax
	movq	.L5(,%rax,8), %rax
	jmp	*%rax
	.section	.rodata
	.align 8
	.align 4
.L5:
	.quad	.L4
	.quad	.L6
	.quad	.L7
	.quad	.L8
	.quad	.L9
	.quad	.L10
	.quad	.L11
	.quad	.L12
	.text
.L4:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_begin_handler
	jmp	.L2
.L6:
	leaq	-16(%rbp), %rcx
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rsi
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	dfa_check_comment_start_handler
	jmp	.L2
.L7:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_comment_handler
	jmp	.L2
.L8:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_check_comment_end_handler
	jmp	.L2
.L9:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_character_constant_handler
	jmp	.L2
.L10:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_character_backslash_handler
	jmp	.L2
.L11:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_string_constant_handler
	jmp	.L2
.L12:
	leaq	-20(%rbp), %rdx
	leaq	-24(%rbp), %rcx
	movl	-12(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	dfa_string_backslash_handler
	jmp	.L2
.L3:
	movl	$__PRETTY_FUNCTION__.2236, %ecx
	movl	$221, %edx
	movl	$.LC0, %esi
	movl	$.LC1, %edi
	call	__assert_fail
.L2:
	call	getchar
	movl	%eax, -12(%rbp)
	cmpl	$-1, -12(%rbp)
	jne	.L13
	leaq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	is_dfa_error
	testl	%eax, %eax
	je	.L14
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	decomment_error_handler
	jmp	.L16
.L14:
	leaq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	decomment_success_handler
.L16:
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L17
	call	__stack_chk_fail
.L17:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	increase_line
	.type	increase_line, @function
increase_line:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	leal	1(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movl	$10, %edi
	call	putchar
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	increase_line, .-increase_line
	.globl	dfa_begin_handler
	.type	dfa_begin_handler, @function
dfa_begin_handler:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.L21
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	jmp	.L20
.L21:
	cmpl	$47, -4(%rbp)
	jne	.L23
	movq	-16(%rbp), %rax
	movl	$1, (%rax)
	jmp	.L20
.L23:
	cmpl	$39, -4(%rbp)
	jne	.L24
	movq	-16(%rbp), %rax
	movl	$4, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L20
.L24:
	cmpl	$34, -4(%rbp)
	jne	.L25
	movq	-16(%rbp), %rax
	movl	$6, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L20
.L25:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	nop
.L20:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	dfa_begin_handler, .-dfa_begin_handler
	.section	.rodata
.LC2:
	.string	"/%c"
	.text
	.globl	dfa_check_comment_start_handler
	.type	dfa_check_comment_start_handler, @function
dfa_check_comment_start_handler:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	cmpl	$42, -4(%rbp)
	jne	.L28
	movl	$32, %edi
	call	putchar
	movq	-16(%rbp), %rax
	movl	$2, (%rax)
	movq	-24(%rbp), %rax
	movl	(%rax), %edx
	movq	-32(%rbp), %rax
	movl	%edx, (%rax)
	jmp	.L27
.L28:
	cmpl	$47, -4(%rbp)
	jne	.L30
	movl	$47, %edi
	call	putchar
	jmp	.L27
.L30:
	cmpl	$10, -4(%rbp)
	jne	.L31
	movl	$47, %edi
	call	putchar
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	jmp	.L27
.L31:
	cmpl	$39, -4(%rbp)
	jne	.L32
	movq	-16(%rbp), %rax
	movl	$4, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	jmp	.L27
.L32:
	cmpl	$34, -4(%rbp)
	jne	.L33
	movq	-16(%rbp), %rax
	movl	$6, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	jmp	.L27
.L33:
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	nop
.L27:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	dfa_check_comment_start_handler, .-dfa_check_comment_start_handler
	.globl	dfa_comment_handler
	.type	dfa_comment_handler, @function
dfa_comment_handler:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$42, -4(%rbp)
	jne	.L36
	movq	-16(%rbp), %rax
	movl	$3, (%rax)
	jmp	.L39
.L36:
	cmpl	$10, -4(%rbp)
	jne	.L39
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	nop
.L39:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	dfa_comment_handler, .-dfa_comment_handler
	.globl	dfa_check_comment_end_handler
	.type	dfa_check_comment_end_handler, @function
dfa_check_comment_end_handler:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$47, -4(%rbp)
	jne	.L41
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	jmp	.L40
.L41:
	cmpl	$42, -4(%rbp)
	je	.L46
	cmpl	$10, -4(%rbp)
	jne	.L45
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	movq	-16(%rbp), %rax
	movl	$2, (%rax)
	jmp	.L40
.L45:
	movq	-16(%rbp), %rax
	movl	$2, (%rax)
	nop
	jmp	.L40
.L46:
	nop
.L40:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	dfa_check_comment_end_handler, .-dfa_check_comment_end_handler
	.globl	dfa_character_constant_handler
	.type	dfa_character_constant_handler, @function
dfa_character_constant_handler:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.L48
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	jmp	.L47
.L48:
	cmpl	$92, -4(%rbp)
	jne	.L50
	movq	-16(%rbp), %rax
	movl	$5, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L47
.L50:
	cmpl	$39, -4(%rbp)
	jne	.L51
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L47
.L51:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	nop
.L47:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	dfa_character_constant_handler, .-dfa_character_constant_handler
	.globl	dfa_character_backslash_handler
	.type	dfa_character_backslash_handler, @function
dfa_character_backslash_handler:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.L54
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	jmp	.L55
.L54:
	cmpl	$92, -4(%rbp)
	jne	.L56
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L53
.L56:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
.L55:
	movq	-16(%rbp), %rax
	movl	$4, (%rax)
	nop
.L53:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	dfa_character_backslash_handler, .-dfa_character_backslash_handler
	.globl	dfa_string_constant_handler
	.type	dfa_string_constant_handler, @function
dfa_string_constant_handler:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.L59
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	jmp	.L58
.L59:
	cmpl	$92, -4(%rbp)
	jne	.L61
	movq	-16(%rbp), %rax
	movl	$7, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L58
.L61:
	cmpl	$34, -4(%rbp)
	jne	.L62
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L58
.L62:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	nop
.L58:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	dfa_string_constant_handler, .-dfa_string_constant_handler
	.globl	dfa_string_backslash_handler
	.type	dfa_string_backslash_handler, @function
dfa_string_backslash_handler:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.L65
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	increase_line
	jmp	.L66
.L65:
	cmpl	$92, -4(%rbp)
	jne	.L67
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
	jmp	.L64
.L67:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	putchar
.L66:
	movq	-16(%rbp), %rax
	movl	$6, (%rax)
	nop
.L64:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	dfa_string_backslash_handler, .-dfa_string_backslash_handler
	.globl	is_dfa_error
	.type	is_dfa_error, @function
is_dfa_error:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$2, %eax
	je	.L70
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$3, %eax
	jne	.L71
.L70:
	movq	-8(%rbp), %rax
	movl	$9, (%rax)
	movl	$1, %eax
	jmp	.L72
.L71:
	movl	$0, %eax
.L72:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	is_dfa_error, .-is_dfa_error
	.section	.rodata
	.align 8
.LC3:
	.string	"Error: line %d: unterminated comment\n"
	.text
	.globl	decomment_error_handler
	.type	decomment_error_handler, @function
decomment_error_handler:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	stderr(%rip), %rax
	movl	-4(%rbp), %edx
	movl	$.LC3, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	decomment_error_handler, .-decomment_error_handler
	.globl	decomment_success_handler
	.type	decomment_success_handler, @function
decomment_success_handler:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$8, (%rax)
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	decomment_success_handler, .-decomment_success_handler
	.section	.rodata
	.type	__PRETTY_FUNCTION__.2236, @object
	.size	__PRETTY_FUNCTION__.2236, 5
__PRETTY_FUNCTION__.2236:
	.string	"main"
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.12) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
