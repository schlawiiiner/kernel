	.file	"apic.c"
	.text
	.type	remap_timer, @function
remap_timer:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movl	$4276093728, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movb	$0, %al
	movl	%eax, %edx
	movzbl	-20(%rbp), %eax
	orl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	remap_timer, .-remap_timer
	.type	unmask_timer, @function
unmask_timer:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$4276093728, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	andl	$-65537, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	unmask_timer, .-unmask_timer
	.type	set_timer_mode, @function
set_timer_mode:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movl	$4276093728, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	andl	$-393217, %eax
	movl	%eax, %edx
	movzbl	-20(%rbp), %eax
	sall	$17, %eax
	andl	$393216, %eax
	orl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	set_timer_mode, .-set_timer_mode
	.type	set_initial_count, @function
set_initial_count:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, -20(%rbp)
	movl	$4276093824, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %edx
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	set_initial_count, .-set_initial_count
	.type	set_divide_configuration, @function
set_divide_configuration:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movl	$4276093920, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %edx
	movzbl	-20(%rbp), %eax
	orl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	set_divide_configuration, .-set_divide_configuration
	.section	.rodata
.LC0:
	.string	"ERROR: No MADT table present"
.LC1:
	.string	"Processor "
.LC2:
	.string	": online capable\n"
.LC3:
	.string	": not online capable\n"
	.align 8
.LC4:
	.string	"\nERROR: cannot parse MADT entry of type: "
	.text
	.globl	parse_MADT
	.type	parse_MADT, @function
parse_MADT:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movabsq	$acpi, %rax
	movq	(%rax), %rax
	andl	$1, %eax
	testq	%rax, %rax
	jne	.L7
	movabsq	$.LC0, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L8:
	nop
	jmp	.L8
.L7:
	movabsq	$acpi, %rax
	movq	8(%rax), %rax
	movq	%rax, -16(%rbp)
	movl	$44, -4(%rbp)
	jmp	.L9
.L23:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	cmpl	$9, %eax
	ja	.L10
	movl	%eax, %eax
	leaq	0(,%rax,8), %rdx
	movabsq	$.L12, %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	notrack jmp	*%rax
	.section	.rodata
	.align 8
	.align 4
.L12:
	.quad	.L18
	.quad	.L17
	.quad	.L16
	.quad	.L15
	.quad	.L14
	.quad	.L13
	.quad	.L10
	.quad	.L10
	.quad	.L10
	.quad	.L11
	.text
.L18:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -72(%rbp)
	movabsq	$.LC1, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	movq	-72(%rbp), %rax
	movzbl	3(%rax), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	movabsq	$printdec, %rax
	call	*%rax
	movq	-72(%rbp), %rax
	movl	4(%rax), %eax
	andl	$3, %eax
	testl	%eax, %eax
	je	.L19
	movabsq	$.LC2, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	jmp	.L20
.L19:
	movabsq	$.LC3, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L20:
	movq	-72(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L17:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	movabsq	$map_IOAPIC_MMIO, %rax
	call	*%rax
	movq	-64(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L16:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L15:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L14:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L13:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L11:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movzbl	1(%rax), %eax
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	jmp	.L9
.L10:
	movl	$0, %esi
	movl	$16711680, %edi
	movabsq	$set_color, %rax
	call	*%rax
	movabsq	$.LC4, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	movabsq	$printhex, %rax
	call	*%rax
.L22:
	nop
	jmp	.L22
.L9:
	movabsq	$acpi, %rax
	movq	8(%rax), %rax
	movl	4(%rax), %eax
	movl	-4(%rbp), %edx
	cmpl	%eax, %edx
	jb	.L23
	nop
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	parse_MADT, .-parse_MADT
	.globl	set_delivery_mode
	.type	set_delivery_mode, @function
set_delivery_mode:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %eax
	cltq
	movl	$4276092928, %edx
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	andb	$-8, %ah
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	-24(%rbp), %edx
	sall	$8, %edx
	andl	$1792, %edx
	orl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	set_delivery_mode, .-set_delivery_mode
	.globl	init_err
	.type	init_err, @function
init_err:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$4276093808, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
	andl	$-33024, -12(%rbp)
	orl	$34, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	init_err, .-init_err
	.section	.rodata
	.align 8
.LC5:
	.string	"ERROR: timeout while waiting to send IPI"
	.text
	.globl	send_IPI
	.type	send_IPI, @function
send_IPI:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	%edx, -44(%rbp)
	movl	$4276093712, %eax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	andl	$16777215, -20(%rbp)
	movl	-36(%rbp), %eax
	sall	$24, %eax
	orl	%eax, -20(%rbp)
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movl	%edx, (%rax)
	movl	$4276093696, %eax
	movq	%rax, -16(%rbp)
	movl	$1048576, -4(%rbp)
	jmp	.L27
.L30:
	cmpl	$0, -4(%rbp)
	jne	.L28
	movabsq	$.LC5, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L29:
	nop
	jmp	.L29
.L28:
	subl	$1, -4(%rbp)
.L27:
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	andl	$4096, %eax
	testl	%eax, %eax
	jne	.L30
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	andl	$-839680, -20(%rbp)
	movl	-40(%rbp), %eax
	movzbl	%al, %eax
	orl	-44(%rbp), %eax
	orl	%eax, -20(%rbp)
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movl	%edx, (%rax)
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	send_IPI, .-send_IPI
	.section	.rodata
.LC6:
	.string	"\nAPIC ERROR\n\n"
.LC7:
	.string	"Send Checksum Error\n"
.LC8:
	.string	"Receive Checksum Error\n"
.LC9:
	.string	"Send Accept Error\n"
.LC10:
	.string	"Receive Accept Error\n"
.LC11:
	.string	"Redirectable IPI\n"
.LC12:
	.string	"Send Illegal Vector\n"
.LC13:
	.string	"Receive Illegal Vector\n"
.LC14:
	.string	"Illegal Register Address\n"
	.text
	.globl	apic_err
	.type	apic_err, @function
apic_err:
.LFB15:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, %esi
	movl	$16711680, %edi
	movabsq	$set_color, %rax
	call	*%rax
	movl	$0, %edi
	movabsq	$fill_screen, %rax
	call	*%rax
	movl	$0, %esi
	movl	$0, %edi
	movabsq	$set_cursor, %rax
	call	*%rax
	movabsq	$.LC6, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	movl	$4276093568, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$0, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L32
	movabsq	$.LC7, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L32:
	movl	-12(%rbp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	.L33
	movabsq	$.LC8, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L33:
	movl	-12(%rbp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L34
	movabsq	$.LC9, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L34:
	movl	-12(%rbp), %eax
	andl	$8, %eax
	testl	%eax, %eax
	je	.L35
	movabsq	$.LC10, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L35:
	movl	-12(%rbp), %eax
	andl	$16, %eax
	testl	%eax, %eax
	je	.L36
	movabsq	$.LC11, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L36:
	movl	-12(%rbp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	je	.L37
	movabsq	$.LC12, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L37:
	movl	-12(%rbp), %eax
	andl	$64, %eax
	testl	%eax, %eax
	je	.L38
	movabsq	$.LC13, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L38:
	movl	-12(%rbp), %eax
	andl	$128, %eax
	testl	%eax, %eax
	je	.L40
	movabsq	$.LC14, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	nop
.L40:
	nop
	jmp	.L40
	.cfi_endproc
.LFE15:
	.size	apic_err, .-apic_err
	.section	.rodata
.LC15:
	.string	" : "
.LC16:
	.string	"\n"
	.text
	.globl	dump_apic_regs
	.type	dump_apic_regs, @function
dump_apic_regs:
.LFB16:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$4276092928, %eax
	movq	%rax, -16(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L42
.L43:
	movl	-4(%rbp), %eax
	cltq
	movl	$4276092928, %edx
	addq	%rdx, %rax
	movq	%rax, %rdi
	movabsq	$printhex, %rax
	call	*%rax
	movabsq	$.LC15, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, %eax
	movq	%rax, %rdi
	movabsq	$printhex, %rax
	call	*%rax
	movabsq	$.LC16, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	addl	$16, -4(%rbp)
.L42:
	cmpl	$1023, -4(%rbp)
	jle	.L43
	movl	$0, -8(%rbp)
	jmp	.L44
.L45:
	movl	-8(%rbp), %eax
	cltq
	movl	$4276092928, %edx
	addq	%rdx, %rax
	movq	%rax, %rdi
	movabsq	$write_hex_to_serial, %rax
	call	*%rax
	movabsq	$.LC15, %rax
	movq	%rax, %rdi
	movabsq	$write_string_to_serial, %rax
	call	*%rax
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, %eax
	movq	%rax, %rdi
	movabsq	$write_hex_to_serial, %rax
	call	*%rax
	movabsq	$.LC16, %rax
	movq	%rax, %rdi
	movabsq	$write_string_to_serial, %rax
	call	*%rax
	addl	$16, -8(%rbp)
.L44:
	cmpl	$1023, -8(%rbp)
	jle	.L45
	nop
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	dump_apic_regs, .-dump_apic_regs
	.globl	set_timer
	.type	set_timer, @function
set_timer:
.LFB17:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$32, %edi
	movabsq	$remap_timer, %rax
	call	*%rax
	movabsq	$unmask_timer, %rax
	call	*%rax
	movl	$1, %edi
	movabsq	$set_timer_mode, %rax
	call	*%rax
	movl	$0, %edi
	movabsq	$set_divide_configuration, %rax
	call	*%rax
	movl	$15728640, %edi
	movabsq	$set_initial_count, %rax
	call	*%rax
	nop
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	set_timer, .-set_timer
	.section	.rodata
.LC17:
	.string	"cpu "
.LC18:
	.string	": online\n"
	.text
	.globl	init_aps
	.type	init_aps, @function
init_aps:
.LFB18:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movabsq	$vacant, %rax
	movb	$0, (%rax)
	movabsq	$count, %rax
	movb	$1, (%rax)
	movl	$836864, %edx
	movl	$0, %esi
	movl	$0, %edi
	movabsq	$send_IPI, %rax
	call	*%rax
	movl	$1048576, -4(%rbp)
	jmp	.L48
.L49:
	subl	$1, -4(%rbp)
.L48:
	cmpl	$0, -4(%rbp)
	jne	.L49
	movabsq	$trampoline_start, %rax
	shrq	$12, %rax
	movzbl	%al, %eax
	movl	$804352, %edx
	movl	%eax, %esi
	movl	$0, %edi
	movabsq	$send_IPI, %rax
	call	*%rax
	movl	$2097152, -4(%rbp)
	jmp	.L50
.L51:
	subl	$1, -4(%rbp)
.L50:
	cmpl	$0, -4(%rbp)
	jne	.L51
	movabsq	$trampoline_start, %rax
	shrq	$12, %rax
	movzbl	%al, %eax
	movl	$804352, %edx
	movl	%eax, %esi
	movl	$0, %edi
	movabsq	$send_IPI, %rax
	call	*%rax
	movl	$2097152, -4(%rbp)
	jmp	.L52
.L53:
	subl	$1, -4(%rbp)
.L52:
	cmpl	$0, -4(%rbp)
	jne	.L53
	nop
.L54:
	movabsq	$vacant, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L54
	movl	$0, -8(%rbp)
	jmp	.L55
.L57:
	movabsq	$cpus, %rax
	movq	(%rax), %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$4, %rax
	subq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	addq	$10, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L56
	movabsq	$.LC17, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	movabsq	$cpus, %rax
	movq	(%rax), %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$4, %rax
	subq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	addq	$8, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	movabsq	$printdec, %rax
	call	*%rax
	movabsq	$.LC18, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L56:
	addl	$1, -8(%rbp)
.L55:
	movabsq	$cpus, %rax
	movq	(%rax), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	cmpl	%eax, -8(%rbp)
	jl	.L57
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	init_aps, .-init_aps
	.section	.rodata
.LC19:
	.string	"no APIC"
.LC20:
	.string	"no MSR"
	.align 8
.LC21:
	.string	"unexpected error wile enabling APIC"
	.text
	.globl	init_APIC
	.type	init_APIC, @function
init_APIC:
.LFB19:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movabsq	$enable_cpu_features, %rax
	call	*%rax
	movl	$0, %ecx
	movl	$2097152, %edx
	movl	$4276092928, %eax
	movq	%rax, %rsi
	movl	$4276092928, %eax
	movq	%rax, %rdi
	movabsq	$map_to, %rax
	call	*%rax
	movabsq	$enable_APIC, %rax
	call	*%rax
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.L60
	cmpl	$1, -4(%rbp)
	jne	.L61
	movabsq	$.LC19, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	jmp	.L65
.L61:
	cmpl	$2, -4(%rbp)
	jne	.L63
	movabsq	$.LC20, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	jmp	.L65
.L63:
	movabsq	$.LC21, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	nop
.L65:
	nop
	jmp	.L65
.L60:
	movl	$-18874368, %edi
	movabsq	$remap_APIC_registers, %rax
	call	*%rax
#APP
# 230 "src/kernel/apic.c" 1
	int $0x1
# 0 "" 2
#NO_APP
	movl	$4276092960, %eax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	shrl	$24, %eax
	movb	%al, -17(%rbp)
	movzbl	-17(%rbp), %eax
	movl	%eax, %edi
	movabsq	$assign_per_core_struct, %rax
	call	*%rax
	movl	$4276093168, %eax
	movq	%rax, -32(%rbp)
	movl	$4276093776, %eax
	movq	%rax, -40(%rbp)
	movl	$4276093792, %eax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rax
	movl	$288, (%rax)
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	andl	$-65537, %eax
	movl	%eax, %edx
	movq	-40(%rbp), %rax
	movl	%edx, (%rax)
	movq	-48(%rbp), %rax
	movl	(%rax), %eax
	andl	$-65537, %eax
	movl	%eax, %edx
	movq	-48(%rbp), %rax
	movl	%edx, (%rax)
	movl	$0, %eax
	movabsq	$init_err, %rdx
	call	*%rdx
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	init_APIC, .-init_APIC
	.section	.rodata
	.align 8
.LC22:
	.string	"LAPIC IRR: Interrupt is pending!\n"
	.align 8
.LC23:
	.string	"LAPIC IRR: Interrupt not received.\n"
	.text
	.globl	check_lapic_irr
	.type	check_lapic_irr, @function
check_lapic_irr:
.LFB20:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movzbl	-20(%rbp), %eax
	shrb	$5, %al
	movzbl	%al, %eax
	addl	$32, %eax
	sall	$4, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4276092928, %edx
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -8(%rbp)
	movzbl	-20(%rbp), %eax
	andl	$31, %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	andl	-8(%rbp), %eax
	testl	%eax, %eax
	je	.L67
	movabsq	$.LC22, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	jmp	.L69
.L67:
	movabsq	$.LC23, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L69:
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	check_lapic_irr, .-check_lapic_irr
	.section	.rodata
	.align 8
.LC24:
	.string	"LAPIC ISR: Interrupt vector is currently being serviced!\n"
	.align 8
.LC25:
	.string	"LAPIC ISR: Interrupt vector is not active.\n"
	.text
	.globl	check_lapic_isr
	.type	check_lapic_isr, @function
check_lapic_isr:
.LFB21:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movzbl	-20(%rbp), %eax
	shrb	$5, %al
	movzbl	%al, %eax
	addl	$16, %eax
	sall	$4, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4276092928, %edx
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -8(%rbp)
	movzbl	-20(%rbp), %eax
	andl	$31, %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	andl	-8(%rbp), %eax
	testl	%eax, %eax
	je	.L71
	movabsq	$.LC24, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
	jmp	.L73
.L71:
	movabsq	$.LC25, %rax
	movq	%rax, %rdi
	movabsq	$print, %rax
	call	*%rax
.L73:
	nop
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	check_lapic_isr, .-check_lapic_isr
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
