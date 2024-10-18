global jump_usermode
section .text

bits 64
jump_usermode:
	mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret

	; set up the stack frame iret expects
	mov rax, rsp
	push (4 * 8) | 3 ; data selector
	push rax ; current rsp
	pushfq ; eflags
	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	mov rax, user_function
	push rax ; instruction address to return to
	iretq

user_function:
	jmp user_function