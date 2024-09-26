bits 64
extern division_error
extern debug
extern non_maskable_interrupt
extern breakpoint
extern overflow
extern bound_range_exceeded
extern invalid_opcode
extern device_not_available
extern double_fault
extern coprocessor_segment_overrun
extern invalid_tss 
extern segment_not_present
extern stack_segment_fault
extern general_protection_fault
extern page_fault
extern x87_floating_point_exception
extern alignment_check
extern machine_check
extern simd_floating_point_exception
extern virtualization_exception
extern control_protection_exception
extern hypervisor_injection_exception
extern vmm_communication_exception
extern security_exception
extern fpu_error_interrupt

%macro PUSH_ALL_REGS 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POP_ALL_REGS 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

section .text
int0:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call division_error
    POP_ALL_REGS
    iret
int1:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call debug
    POP_ALL_REGS
    iret
int2:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call non_maskable_interrupt
    POP_ALL_REGS
    iret
int3:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call breakpoint
    POP_ALL_REGS
    iret
int4:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call overflow
    POP_ALL_REGS
    iret
int5:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call bound_range_exceeded
    POP_ALL_REGS
    iret
int6:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call invalid_opcode
    POP_ALL_REGS
    iret
int7:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call device_not_available
    POP_ALL_REGS
    iret
int8:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call double_fault
    POP_ALL_REGS
    iret
int9:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call coprocessor_segment_overrun
    POP_ALL_REGS
    iret
int10:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call invalid_tss
    POP_ALL_REGS
    iret
int11:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call segment_not_present
    POP_ALL_REGS
    iret
int12:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call stack_segment_fault
    POP_ALL_REGS
    iret
int13:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call general_protection_fault
    POP_ALL_REGS
    iret
int14:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call page_fault
    POP_ALL_REGS
    iret
int15:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 15
	mov rax, [irq_handlers+120]
	call rax
	POP_ALL_REGS
	iretq
int16:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call x87_floating_point_exception
    POP_ALL_REGS
    iret
int17:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call alignment_check
    POP_ALL_REGS
    iret
int18:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call machine_check
    POP_ALL_REGS
    iret
int19:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call simd_floating_point_exception
    POP_ALL_REGS
    iret
int20:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call virtualization_exception
    POP_ALL_REGS
    iret
int21:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call control_protection_exception
    POP_ALL_REGS
    iret
int22:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 22
	mov rax, [irq_handlers+176]
	call rax
	POP_ALL_REGS
	iretq
int23:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 23
	mov rax, [irq_handlers+184]
	call rax
	POP_ALL_REGS
	iretq
int24:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 24
	mov rax, [irq_handlers+192]
	call rax
	POP_ALL_REGS
	iretq
int25:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 25
	mov rax, [irq_handlers+200]
	call rax
	POP_ALL_REGS
	iretq
int26:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 26
	mov rax, [irq_handlers+208]
	call rax
	POP_ALL_REGS
	iretq
int27:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 27
	mov rax, [irq_handlers+216]
	call rax
	POP_ALL_REGS
	iretq
int28:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call hypervisor_injection_exception
    POP_ALL_REGS
    iret
int29:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call vmm_communication_exception
    POP_ALL_REGS
    iret
int30:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call security_exception
    POP_ALL_REGS
    iret
int31:
    cli
	cld
	PUSH_ALL_REGS
	mov rdi, 31
	mov rax, [irq_handlers+248]
	call rax
	POP_ALL_REGS
	iretq

; ---------------------------------------- IRQ ------------------------------------------------------
int32:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 32
	mov rax, [irq_handlers+256]
	call rax
	POP_ALL_REGS
	iretq
int33:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 33
	mov rax, [irq_handlers+264]
	call rax
	POP_ALL_REGS
	iretq
int34:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 34
	mov rax, [irq_handlers+272]
	call rax
	POP_ALL_REGS
	iretq
int35:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 35
	mov rax, [irq_handlers+280]
	call rax
	POP_ALL_REGS
	iretq
int36:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 36
	mov rax, [irq_handlers+288]
	call rax
	POP_ALL_REGS
	iretq
int37:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 37
	mov rax, [irq_handlers+296]
	call rax
	POP_ALL_REGS
	iretq
int38:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 38
	mov rax, [irq_handlers+304]
	call rax
	POP_ALL_REGS
	iretq
int39:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 39
	mov rax, [irq_handlers+312]
	call rax
	POP_ALL_REGS
	iretq
int40:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 40
	mov rax, [irq_handlers+320]
	call rax
	POP_ALL_REGS
	iretq
int41:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 41
	mov rax, [irq_handlers+328]
	call rax
	POP_ALL_REGS
	iretq
int42:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 42
	mov rax, [irq_handlers+336]
	call rax
	POP_ALL_REGS
	iretq
int43:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 43
	mov rax, [irq_handlers+344]
	call rax
	POP_ALL_REGS
	iretq
int44:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 44
	mov rax, [irq_handlers+352]
	call rax
	POP_ALL_REGS
	iretq
int45:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 45
	mov rax, [irq_handlers+360]
	call rax
	POP_ALL_REGS
	iretq
int46:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 46
	mov rax, [irq_handlers+368]
	call rax
	POP_ALL_REGS
	iretq
int47:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 47
	mov rax, [irq_handlers+376]
	call rax
	POP_ALL_REGS
	iretq
int48:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 48
	mov rax, [irq_handlers+384]
	call rax
	POP_ALL_REGS
	iretq
int49:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 49
	mov rax, [irq_handlers+392]
	call rax
	POP_ALL_REGS
	iretq
int50:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 50
	mov rax, [irq_handlers+400]
	call rax
	POP_ALL_REGS
	iretq
int51:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 51
	mov rax, [irq_handlers+408]
	call rax
	POP_ALL_REGS
	iretq
int52:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 52
	mov rax, [irq_handlers+416]
	call rax
	POP_ALL_REGS
	iretq
int53:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 53
	mov rax, [irq_handlers+424]
	call rax
	POP_ALL_REGS
	iretq
int54:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 54
	mov rax, [irq_handlers+432]
	call rax
	POP_ALL_REGS
	iretq
int55:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 55
	mov rax, [irq_handlers+440]
	call rax
	POP_ALL_REGS
	iretq
int56:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 56
	mov rax, [irq_handlers+448]
	call rax
	POP_ALL_REGS
	iretq
int57:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 57
	mov rax, [irq_handlers+456]
	call rax
	POP_ALL_REGS
	iretq
int58:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 58
	mov rax, [irq_handlers+464]
	call rax
	POP_ALL_REGS
	iretq
int59:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 59
	mov rax, [irq_handlers+472]
	call rax
	POP_ALL_REGS
	iretq
int60:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 60
	mov rax, [irq_handlers+480]
	call rax
	POP_ALL_REGS
	iretq
int61:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 61
	mov rax, [irq_handlers+488]
	call rax
	POP_ALL_REGS
	iretq
int62:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 62
	mov rax, [irq_handlers+496]
	call rax
	POP_ALL_REGS
	iretq
int63:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 63
	mov rax, [irq_handlers+504]
	call rax
	POP_ALL_REGS
	iretq
int64:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 64
	mov rax, [irq_handlers+512]
	call rax
	POP_ALL_REGS
	iretq
int65:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 65
	mov rax, [irq_handlers+520]
	call rax
	POP_ALL_REGS
	iretq
int66:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 66
	mov rax, [irq_handlers+528]
	call rax
	POP_ALL_REGS
	iretq
int67:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 67
	mov rax, [irq_handlers+536]
	call rax
	POP_ALL_REGS
	iretq
int68:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 68
	mov rax, [irq_handlers+544]
	call rax
	POP_ALL_REGS
	iretq
int69:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 69
	mov rax, [irq_handlers+552]
	call rax
	POP_ALL_REGS
	iretq
int70:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 70
	mov rax, [irq_handlers+560]
	call rax
	POP_ALL_REGS
	iretq
int71:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 71
	mov rax, [irq_handlers+568]
	call rax
	POP_ALL_REGS
	iretq
int72:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 72
	mov rax, [irq_handlers+576]
	call rax
	POP_ALL_REGS
	iretq
int73:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 73
	mov rax, [irq_handlers+584]
	call rax
	POP_ALL_REGS
	iretq
int74:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 74
	mov rax, [irq_handlers+592]
	call rax
	POP_ALL_REGS
	iretq
int75:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 75
	mov rax, [irq_handlers+600]
	call rax
	POP_ALL_REGS
	iretq
int76:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 76
	mov rax, [irq_handlers+608]
	call rax
	POP_ALL_REGS
	iretq
int77:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 77
	mov rax, [irq_handlers+616]
	call rax
	POP_ALL_REGS
	iretq
int78:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 78
	mov rax, [irq_handlers+624]
	call rax
	POP_ALL_REGS
	iretq
int79:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 79
	mov rax, [irq_handlers+632]
	call rax
	POP_ALL_REGS
	iretq
int80:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 80
	mov rax, [irq_handlers+640]
	call rax
	POP_ALL_REGS
	iretq
int81:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 81
	mov rax, [irq_handlers+648]
	call rax
	POP_ALL_REGS
	iretq
int82:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 82
	mov rax, [irq_handlers+656]
	call rax
	POP_ALL_REGS
	iretq
int83:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 83
	mov rax, [irq_handlers+664]
	call rax
	POP_ALL_REGS
	iretq
int84:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 84
	mov rax, [irq_handlers+672]
	call rax
	POP_ALL_REGS
	iretq
int85:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 85
	mov rax, [irq_handlers+680]
	call rax
	POP_ALL_REGS
	iretq
int86:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 86
	mov rax, [irq_handlers+688]
	call rax
	POP_ALL_REGS
	iretq
int87:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 87
	mov rax, [irq_handlers+696]
	call rax
	POP_ALL_REGS
	iretq
int88:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 88
	mov rax, [irq_handlers+704]
	call rax
	POP_ALL_REGS
	iretq
int89:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 89
	mov rax, [irq_handlers+712]
	call rax
	POP_ALL_REGS
	iretq
int90:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 90
	mov rax, [irq_handlers+720]
	call rax
	POP_ALL_REGS
	iretq
int91:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 91
	mov rax, [irq_handlers+728]
	call rax
	POP_ALL_REGS
	iretq
int92:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 92
	mov rax, [irq_handlers+736]
	call rax
	POP_ALL_REGS
	iretq
int93:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 93
	mov rax, [irq_handlers+744]
	call rax
	POP_ALL_REGS
	iretq
int94:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 94
	mov rax, [irq_handlers+752]
	call rax
	POP_ALL_REGS
	iretq
int95:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 95
	mov rax, [irq_handlers+760]
	call rax
	POP_ALL_REGS
	iretq
int96:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 96
	mov rax, [irq_handlers+768]
	call rax
	POP_ALL_REGS
	iretq
int97:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 97
	mov rax, [irq_handlers+776]
	call rax
	POP_ALL_REGS
	iretq
int98:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 98
	mov rax, [irq_handlers+784]
	call rax
	POP_ALL_REGS
	iretq
int99:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 99
	mov rax, [irq_handlers+792]
	call rax
	POP_ALL_REGS
	iretq
int100:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 100
	mov rax, [irq_handlers+800]
	call rax
	POP_ALL_REGS
	iretq
int101:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 101
	mov rax, [irq_handlers+808]
	call rax
	POP_ALL_REGS
	iretq
int102:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 102
	mov rax, [irq_handlers+816]
	call rax
	POP_ALL_REGS
	iretq
int103:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 103
	mov rax, [irq_handlers+824]
	call rax
	POP_ALL_REGS
	iretq
int104:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 104
	mov rax, [irq_handlers+832]
	call rax
	POP_ALL_REGS
	iretq
int105:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 105
	mov rax, [irq_handlers+840]
	call rax
	POP_ALL_REGS
	iretq
int106:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 106
	mov rax, [irq_handlers+848]
	call rax
	POP_ALL_REGS
	iretq
int107:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 107
	mov rax, [irq_handlers+856]
	call rax
	POP_ALL_REGS
	iretq
int108:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 108
	mov rax, [irq_handlers+864]
	call rax
	POP_ALL_REGS
	iretq
int109:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 109
	mov rax, [irq_handlers+872]
	call rax
	POP_ALL_REGS
	iretq
int110:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 110
	mov rax, [irq_handlers+880]
	call rax
	POP_ALL_REGS
	iretq
int111:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 111
	mov rax, [irq_handlers+888]
	call rax
	POP_ALL_REGS
	iretq
int112:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 112
	mov rax, [irq_handlers+896]
	call rax
	POP_ALL_REGS
	iretq
int113:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 113
	mov rax, [irq_handlers+904]
	call rax
	POP_ALL_REGS
	iretq
int114:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 114
	mov rax, [irq_handlers+912]
	call rax
	POP_ALL_REGS
	iretq
int115:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 115
	mov rax, [irq_handlers+920]
	call rax
	POP_ALL_REGS
	iretq
int116:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 116
	mov rax, [irq_handlers+928]
	call rax
	POP_ALL_REGS
	iretq
int117:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 117
	mov rax, [irq_handlers+936]
	call rax
	POP_ALL_REGS
	iretq
int118:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 118
	mov rax, [irq_handlers+944]
	call rax
	POP_ALL_REGS
	iretq
int119:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 119
	mov rax, [irq_handlers+952]
	call rax
	POP_ALL_REGS
	iretq
int120:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 120
	mov rax, [irq_handlers+960]
	call rax
	POP_ALL_REGS
	iretq
int121:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 121
	mov rax, [irq_handlers+968]
	call rax
	POP_ALL_REGS
	iretq
int122:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 122
	mov rax, [irq_handlers+976]
	call rax
	POP_ALL_REGS
	iretq
int123:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 123
	mov rax, [irq_handlers+984]
	call rax
	POP_ALL_REGS
	iretq
int124:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 124
	mov rax, [irq_handlers+992]
	call rax
	POP_ALL_REGS
	iretq
int125:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 125
	mov rax, [irq_handlers+1000]
	call rax
	POP_ALL_REGS
	iretq
int126:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 126
	mov rax, [irq_handlers+1008]
	call rax
	POP_ALL_REGS
	iretq
int127:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 127
	mov rax, [irq_handlers+1016]
	call rax
	POP_ALL_REGS
	iretq
int128:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 128
	mov rax, [irq_handlers+1024]
	call rax
	POP_ALL_REGS
	iretq
int129:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 129
	mov rax, [irq_handlers+1032]
	call rax
	POP_ALL_REGS
	iretq
int130:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 130
	mov rax, [irq_handlers+1040]
	call rax
	POP_ALL_REGS
	iretq
int131:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 131
	mov rax, [irq_handlers+1048]
	call rax
	POP_ALL_REGS
	iretq
int132:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 132
	mov rax, [irq_handlers+1056]
	call rax
	POP_ALL_REGS
	iretq
int133:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 133
	mov rax, [irq_handlers+1064]
	call rax
	POP_ALL_REGS
	iretq
int134:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 134
	mov rax, [irq_handlers+1072]
	call rax
	POP_ALL_REGS
	iretq
int135:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 135
	mov rax, [irq_handlers+1080]
	call rax
	POP_ALL_REGS
	iretq
int136:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 136
	mov rax, [irq_handlers+1088]
	call rax
	POP_ALL_REGS
	iretq
int137:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 137
	mov rax, [irq_handlers+1096]
	call rax
	POP_ALL_REGS
	iretq
int138:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 138
	mov rax, [irq_handlers+1104]
	call rax
	POP_ALL_REGS
	iretq
int139:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 139
	mov rax, [irq_handlers+1112]
	call rax
	POP_ALL_REGS
	iretq
int140:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 140
	mov rax, [irq_handlers+1120]
	call rax
	POP_ALL_REGS
	iretq
int141:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 141
	mov rax, [irq_handlers+1128]
	call rax
	POP_ALL_REGS
	iretq
int142:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 142
	mov rax, [irq_handlers+1136]
	call rax
	POP_ALL_REGS
	iretq
int143:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 143
	mov rax, [irq_handlers+1144]
	call rax
	POP_ALL_REGS
	iretq
int144:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 144
	mov rax, [irq_handlers+1152]
	call rax
	POP_ALL_REGS
	iretq
int145:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 145
	mov rax, [irq_handlers+1160]
	call rax
	POP_ALL_REGS
	iretq
int146:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 146
	mov rax, [irq_handlers+1168]
	call rax
	POP_ALL_REGS
	iretq
int147:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 147
	mov rax, [irq_handlers+1176]
	call rax
	POP_ALL_REGS
	iretq
int148:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 148
	mov rax, [irq_handlers+1184]
	call rax
	POP_ALL_REGS
	iretq
int149:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 149
	mov rax, [irq_handlers+1192]
	call rax
	POP_ALL_REGS
	iretq
int150:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 150
	mov rax, [irq_handlers+1200]
	call rax
	POP_ALL_REGS
	iretq
int151:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 151
	mov rax, [irq_handlers+1208]
	call rax
	POP_ALL_REGS
	iretq
int152:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 152
	mov rax, [irq_handlers+1216]
	call rax
	POP_ALL_REGS
	iretq
int153:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 153
	mov rax, [irq_handlers+1224]
	call rax
	POP_ALL_REGS
	iretq
int154:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 154
	mov rax, [irq_handlers+1232]
	call rax
	POP_ALL_REGS
	iretq
int155:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 155
	mov rax, [irq_handlers+1240]
	call rax
	POP_ALL_REGS
	iretq
int156:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 156
	mov rax, [irq_handlers+1248]
	call rax
	POP_ALL_REGS
	iretq
int157:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 157
	mov rax, [irq_handlers+1256]
	call rax
	POP_ALL_REGS
	iretq
int158:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 158
	mov rax, [irq_handlers+1264]
	call rax
	POP_ALL_REGS
	iretq
int159:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 159
	mov rax, [irq_handlers+1272]
	call rax
	POP_ALL_REGS
	iretq
int160:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 160
	mov rax, [irq_handlers+1280]
	call rax
	POP_ALL_REGS
	iretq
int161:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 161
	mov rax, [irq_handlers+1288]
	call rax
	POP_ALL_REGS
	iretq
int162:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 162
	mov rax, [irq_handlers+1296]
	call rax
	POP_ALL_REGS
	iretq
int163:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 163
	mov rax, [irq_handlers+1304]
	call rax
	POP_ALL_REGS
	iretq
int164:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 164
	mov rax, [irq_handlers+1312]
	call rax
	POP_ALL_REGS
	iretq
int165:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 165
	mov rax, [irq_handlers+1320]
	call rax
	POP_ALL_REGS
	iretq
int166:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 166
	mov rax, [irq_handlers+1328]
	call rax
	POP_ALL_REGS
	iretq
int167:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 167
	mov rax, [irq_handlers+1336]
	call rax
	POP_ALL_REGS
	iretq
int168:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 168
	mov rax, [irq_handlers+1344]
	call rax
	POP_ALL_REGS
	iretq
int169:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 169
	mov rax, [irq_handlers+1352]
	call rax
	POP_ALL_REGS
	iretq
int170:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 170
	mov rax, [irq_handlers+1360]
	call rax
	POP_ALL_REGS
	iretq
int171:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 171
	mov rax, [irq_handlers+1368]
	call rax
	POP_ALL_REGS
	iretq
int172:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 172
	mov rax, [irq_handlers+1376]
	call rax
	POP_ALL_REGS
	iretq
int173:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 173
	mov rax, [irq_handlers+1384]
	call rax
	POP_ALL_REGS
	iretq
int174:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 174
	mov rax, [irq_handlers+1392]
	call rax
	POP_ALL_REGS
	iretq
int175:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 175
	mov rax, [irq_handlers+1400]
	call rax
	POP_ALL_REGS
	iretq
int176:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 176
	mov rax, [irq_handlers+1408]
	call rax
	POP_ALL_REGS
	iretq
int177:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 177
	mov rax, [irq_handlers+1416]
	call rax
	POP_ALL_REGS
	iretq
int178:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 178
	mov rax, [irq_handlers+1424]
	call rax
	POP_ALL_REGS
	iretq
int179:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 179
	mov rax, [irq_handlers+1432]
	call rax
	POP_ALL_REGS
	iretq
int180:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 180
	mov rax, [irq_handlers+1440]
	call rax
	POP_ALL_REGS
	iretq
int181:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 181
	mov rax, [irq_handlers+1448]
	call rax
	POP_ALL_REGS
	iretq
int182:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 182
	mov rax, [irq_handlers+1456]
	call rax
	POP_ALL_REGS
	iretq
int183:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 183
	mov rax, [irq_handlers+1464]
	call rax
	POP_ALL_REGS
	iretq
int184:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 184
	mov rax, [irq_handlers+1472]
	call rax
	POP_ALL_REGS
	iretq
int185:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 185
	mov rax, [irq_handlers+1480]
	call rax
	POP_ALL_REGS
	iretq
int186:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 186
	mov rax, [irq_handlers+1488]
	call rax
	POP_ALL_REGS
	iretq
int187:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 187
	mov rax, [irq_handlers+1496]
	call rax
	POP_ALL_REGS
	iretq
int188:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 188
	mov rax, [irq_handlers+1504]
	call rax
	POP_ALL_REGS
	iretq
int189:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 189
	mov rax, [irq_handlers+1512]
	call rax
	POP_ALL_REGS
	iretq
int190:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 190
	mov rax, [irq_handlers+1520]
	call rax
	POP_ALL_REGS
	iretq
int191:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 191
	mov rax, [irq_handlers+1528]
	call rax
	POP_ALL_REGS
	iretq
int192:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 192
	mov rax, [irq_handlers+1536]
	call rax
	POP_ALL_REGS
	iretq
int193:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 193
	mov rax, [irq_handlers+1544]
	call rax
	POP_ALL_REGS
	iretq
int194:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 194
	mov rax, [irq_handlers+1552]
	call rax
	POP_ALL_REGS
	iretq
int195:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 195
	mov rax, [irq_handlers+1560]
	call rax
	POP_ALL_REGS
	iretq
int196:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 196
	mov rax, [irq_handlers+1568]
	call rax
	POP_ALL_REGS
	iretq
int197:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 197
	mov rax, [irq_handlers+1576]
	call rax
	POP_ALL_REGS
	iretq
int198:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 198
	mov rax, [irq_handlers+1584]
	call rax
	POP_ALL_REGS
	iretq
int199:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 199
	mov rax, [irq_handlers+1592]
	call rax
	POP_ALL_REGS
	iretq
int200:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 200
	mov rax, [irq_handlers+1600]
	call rax
	POP_ALL_REGS
	iretq
int201:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 201
	mov rax, [irq_handlers+1608]
	call rax
	POP_ALL_REGS
	iretq
int202:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 202
	mov rax, [irq_handlers+1616]
	call rax
	POP_ALL_REGS
	iretq
int203:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 203
	mov rax, [irq_handlers+1624]
	call rax
	POP_ALL_REGS
	iretq
int204:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 204
	mov rax, [irq_handlers+1632]
	call rax
	POP_ALL_REGS
	iretq
int205:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 205
	mov rax, [irq_handlers+1640]
	call rax
	POP_ALL_REGS
	iretq
int206:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 206
	mov rax, [irq_handlers+1648]
	call rax
	POP_ALL_REGS
	iretq
int207:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 207
	mov rax, [irq_handlers+1656]
	call rax
	POP_ALL_REGS
	iretq
int208:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 208
	mov rax, [irq_handlers+1664]
	call rax
	POP_ALL_REGS
	iretq
int209:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 209
	mov rax, [irq_handlers+1672]
	call rax
	POP_ALL_REGS
	iretq
int210:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 210
	mov rax, [irq_handlers+1680]
	call rax
	POP_ALL_REGS
	iretq
int211:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 211
	mov rax, [irq_handlers+1688]
	call rax
	POP_ALL_REGS
	iretq
int212:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 212
	mov rax, [irq_handlers+1696]
	call rax
	POP_ALL_REGS
	iretq
int213:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 213
	mov rax, [irq_handlers+1704]
	call rax
	POP_ALL_REGS
	iretq
int214:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 214
	mov rax, [irq_handlers+1712]
	call rax
	POP_ALL_REGS
	iretq
int215:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 215
	mov rax, [irq_handlers+1720]
	call rax
	POP_ALL_REGS
	iretq
int216:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 216
	mov rax, [irq_handlers+1728]
	call rax
	POP_ALL_REGS
	iretq
int217:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 217
	mov rax, [irq_handlers+1736]
	call rax
	POP_ALL_REGS
	iretq
int218:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 218
	mov rax, [irq_handlers+1744]
	call rax
	POP_ALL_REGS
	iretq
int219:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 219
	mov rax, [irq_handlers+1752]
	call rax
	POP_ALL_REGS
	iretq
int220:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 220
	mov rax, [irq_handlers+1760]
	call rax
	POP_ALL_REGS
	iretq
int221:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 221
	mov rax, [irq_handlers+1768]
	call rax
	POP_ALL_REGS
	iretq
int222:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 222
	mov rax, [irq_handlers+1776]
	call rax
	POP_ALL_REGS
	iretq
int223:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 223
	mov rax, [irq_handlers+1784]
	call rax
	POP_ALL_REGS
	iretq
int224:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 224
	mov rax, [irq_handlers+1792]
	call rax
	POP_ALL_REGS
	iretq
int225:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 225
	mov rax, [irq_handlers+1800]
	call rax
	POP_ALL_REGS
	iretq
int226:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 226
	mov rax, [irq_handlers+1808]
	call rax
	POP_ALL_REGS
	iretq
int227:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 227
	mov rax, [irq_handlers+1816]
	call rax
	POP_ALL_REGS
	iretq
int228:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 228
	mov rax, [irq_handlers+1824]
	call rax
	POP_ALL_REGS
	iretq
int229:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 229
	mov rax, [irq_handlers+1832]
	call rax
	POP_ALL_REGS
	iretq
int230:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 230
	mov rax, [irq_handlers+1840]
	call rax
	POP_ALL_REGS
	iretq
int231:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 231
	mov rax, [irq_handlers+1848]
	call rax
	POP_ALL_REGS
	iretq
int232:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 232
	mov rax, [irq_handlers+1856]
	call rax
	POP_ALL_REGS
	iretq
int233:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 233
	mov rax, [irq_handlers+1864]
	call rax
	POP_ALL_REGS
	iretq
int234:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 234
	mov rax, [irq_handlers+1872]
	call rax
	POP_ALL_REGS
	iretq
int235:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 235
	mov rax, [irq_handlers+1880]
	call rax
	POP_ALL_REGS
	iretq
int236:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 236
	mov rax, [irq_handlers+1888]
	call rax
	POP_ALL_REGS
	iretq
int237:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 237
	mov rax, [irq_handlers+1896]
	call rax
	POP_ALL_REGS
	iretq
int238:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 238
	mov rax, [irq_handlers+1904]
	call rax
	POP_ALL_REGS
	iretq
int239:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 239
	mov rax, [irq_handlers+1912]
	call rax
	POP_ALL_REGS
	iretq
int240:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 240
	mov rax, [irq_handlers+1920]
	call rax
	POP_ALL_REGS
	iretq
int241:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 241
	mov rax, [irq_handlers+1928]
	call rax
	POP_ALL_REGS
	iretq
int242:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 242
	mov rax, [irq_handlers+1936]
	call rax
	POP_ALL_REGS
	iretq
int243:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 243
	mov rax, [irq_handlers+1944]
	call rax
	POP_ALL_REGS
	iretq
int244:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 244
	mov rax, [irq_handlers+1952]
	call rax
	POP_ALL_REGS
	iretq
int245:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 245
	mov rax, [irq_handlers+1960]
	call rax
	POP_ALL_REGS
	iretq
int246:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 246
	mov rax, [irq_handlers+1968]
	call rax
	POP_ALL_REGS
	iretq
int247:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 247
	mov rax, [irq_handlers+1976]
	call rax
	POP_ALL_REGS
	iretq
int248:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 248
	mov rax, [irq_handlers+1984]
	call rax
	POP_ALL_REGS
	iretq
int249:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 249
	mov rax, [irq_handlers+1992]
	call rax
	POP_ALL_REGS
	iretq
int250:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 250
	mov rax, [irq_handlers+2000]
	call rax
	POP_ALL_REGS
	iretq
int251:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 251
	mov rax, [irq_handlers+2008]
	call rax
	POP_ALL_REGS
	iretq
int252:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 252
	mov rax, [irq_handlers+2016]
	call rax
	POP_ALL_REGS
	iretq
int253:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 253
	mov rax, [irq_handlers+2024]
	call rax
	POP_ALL_REGS
	iretq
int254:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 254
	mov rax, [irq_handlers+2032]
	call rax
	POP_ALL_REGS
	iretq
int255:
	cli
	cld
	PUSH_ALL_REGS
	mov rdi, 255
	mov rax, [irq_handlers+2040]
	call rax
	POP_ALL_REGS
	iretq


exception_halt_main:
    cli
    hlt
    jmp exception_halt_main

; rax : address of handler function
; rcx : offset of IDT in bytes (e.g. for #DF 0x8*16)
make_entry:
    mov rdx, IDT
    add rdx, rcx
    mov word [rdx], ax
    mov word [rdx+2], CODE_SEG
    mov byte [rdx+4], 0x0
    mov byte [rdx+5], 0x8f
    shr rax, 16
    mov word [rdx+6], ax
    shr rax, 16
    mov dword [rdx+8], eax
    mov dword [rdx+12], 0x0
    ret

make_entry_interrupt:
    mov rdx, IDT
    add rdx, rcx
    mov word [rdx], ax
    mov word [rdx+2], CODE_SEG
    mov byte [rdx+4], 0x0
    mov byte [rdx+5], 0x8e
    shr rax, 16
    mov word [rdx+6], ax
    shr rax, 16
    mov dword [rdx+8], eax
    mov dword [rdx+12], 0x0
    ret

enable_interrupts:
jmp clear_IDT
disable_pic: ; otherwise the PIC-timer would trigger an double fault (it is mapped to 0x8)
    mov dx, 0x21 ;PIC1 DATA
    mov al, 0xff
    out dx, al
    mov dx, 0xa1 ; PIC2 DATA
    out dx, al
clear_IDT:
    xor rax, rax
    mov rcx, 256*2
    mov rdi, IDT
    rep stosq


make_entries:
    mov rax, int0
	mov rcx, 0
	call make_entry
	mov rax, int1
	mov rcx, 16
	call make_entry
	mov rax, int2
	mov rcx, 32
	call make_entry
	mov rax, int3
	mov rcx, 48
	call make_entry
	mov rax, int4
	mov rcx, 64
	call make_entry
	mov rax, int5
	mov rcx, 80
	call make_entry
	mov rax, int6
	mov rcx, 96
	call make_entry
	mov rax, int7
	mov rcx, 112
	call make_entry
	mov rax, int8
	mov rcx, 128
	call make_entry
	mov rax, int9
	mov rcx, 144
	call make_entry
	mov rax, int10
	mov rcx, 160
	call make_entry
	mov rax, int11
	mov rcx, 176
	call make_entry
	mov rax, int12
	mov rcx, 192
	call make_entry
	mov rax, int13
	mov rcx, 208
	call make_entry
	mov rax, int14
	mov rcx, 224
	call make_entry
	mov rax, int15
	mov rcx, 240
	call make_entry
	mov rax, int16
	mov rcx, 256
	call make_entry
	mov rax, int17
	mov rcx, 272
	call make_entry
	mov rax, int18
	mov rcx, 288
	call make_entry
	mov rax, int19
	mov rcx, 304
	call make_entry
	mov rax, int20
	mov rcx, 320
	call make_entry
	mov rax, int21
	mov rcx, 336
	call make_entry
	mov rax, int22
	mov rcx, 352
	call make_entry
	mov rax, int23
	mov rcx, 368
	call make_entry
	mov rax, int24
	mov rcx, 384
	call make_entry
	mov rax, int25
	mov rcx, 400
	call make_entry
	mov rax, int26
	mov rcx, 416
	call make_entry
	mov rax, int27
	mov rcx, 432
	call make_entry
	mov rax, int28
	mov rcx, 448
	call make_entry
	mov rax, int29
	mov rcx, 464
	call make_entry
	mov rax, int30
	mov rcx, 480
	call make_entry
	mov rax, int31
	mov rcx, 496
	call make_entry
	mov rax, int32
	mov rcx, 512
	call make_entry
	mov rax, int33
	mov rcx, 528
	call make_entry
	mov rax, int34
	mov rcx, 544
	call make_entry
	mov rax, int35
	mov rcx, 560
	call make_entry
	mov rax, int36
	mov rcx, 576
	call make_entry
	mov rax, int37
	mov rcx, 592
	call make_entry
	mov rax, int38
	mov rcx, 608
	call make_entry
	mov rax, int39
	mov rcx, 624
	call make_entry
	mov rax, int40
	mov rcx, 640
	call make_entry
	mov rax, int41
	mov rcx, 656
	call make_entry
	mov rax, int42
	mov rcx, 672
	call make_entry
	mov rax, int43
	mov rcx, 688
	call make_entry
	mov rax, int44
	mov rcx, 704
	call make_entry
	mov rax, int45
	mov rcx, 720
	call make_entry
	mov rax, int46
	mov rcx, 736
	call make_entry
	mov rax, int47
	mov rcx, 752
	call make_entry
	mov rax, int48
	mov rcx, 768
	call make_entry
	mov rax, int49
	mov rcx, 784
	call make_entry
	mov rax, int50
	mov rcx, 800
	call make_entry
	mov rax, int51
	mov rcx, 816
	call make_entry
	mov rax, int52
	mov rcx, 832
	call make_entry
	mov rax, int53
	mov rcx, 848
	call make_entry
	mov rax, int54
	mov rcx, 864
	call make_entry
	mov rax, int55
	mov rcx, 880
	call make_entry
	mov rax, int56
	mov rcx, 896
	call make_entry
	mov rax, int57
	mov rcx, 912
	call make_entry
	mov rax, int58
	mov rcx, 928
	call make_entry
	mov rax, int59
	mov rcx, 944
	call make_entry
	mov rax, int60
	mov rcx, 960
	call make_entry
	mov rax, int61
	mov rcx, 976
	call make_entry
	mov rax, int62
	mov rcx, 992
	call make_entry
	mov rax, int63
	mov rcx, 1008
	call make_entry
	mov rax, int64
	mov rcx, 1024
	call make_entry
	mov rax, int65
	mov rcx, 1040
	call make_entry
	mov rax, int66
	mov rcx, 1056
	call make_entry
	mov rax, int67
	mov rcx, 1072
	call make_entry
	mov rax, int68
	mov rcx, 1088
	call make_entry
	mov rax, int69
	mov rcx, 1104
	call make_entry
	mov rax, int70
	mov rcx, 1120
	call make_entry
	mov rax, int71
	mov rcx, 1136
	call make_entry
	mov rax, int72
	mov rcx, 1152
	call make_entry
	mov rax, int73
	mov rcx, 1168
	call make_entry
	mov rax, int74
	mov rcx, 1184
	call make_entry
	mov rax, int75
	mov rcx, 1200
	call make_entry
	mov rax, int76
	mov rcx, 1216
	call make_entry
	mov rax, int77
	mov rcx, 1232
	call make_entry
	mov rax, int78
	mov rcx, 1248
	call make_entry
	mov rax, int79
	mov rcx, 1264
	call make_entry
	mov rax, int80
	mov rcx, 1280
	call make_entry
	mov rax, int81
	mov rcx, 1296
	call make_entry
	mov rax, int82
	mov rcx, 1312
	call make_entry
	mov rax, int83
	mov rcx, 1328
	call make_entry
	mov rax, int84
	mov rcx, 1344
	call make_entry
	mov rax, int85
	mov rcx, 1360
	call make_entry
	mov rax, int86
	mov rcx, 1376
	call make_entry
	mov rax, int87
	mov rcx, 1392
	call make_entry
	mov rax, int88
	mov rcx, 1408
	call make_entry
	mov rax, int89
	mov rcx, 1424
	call make_entry
	mov rax, int90
	mov rcx, 1440
	call make_entry
	mov rax, int91
	mov rcx, 1456
	call make_entry
	mov rax, int92
	mov rcx, 1472
	call make_entry
	mov rax, int93
	mov rcx, 1488
	call make_entry
	mov rax, int94
	mov rcx, 1504
	call make_entry
	mov rax, int95
	mov rcx, 1520
	call make_entry
	mov rax, int96
	mov rcx, 1536
	call make_entry
	mov rax, int97
	mov rcx, 1552
	call make_entry
	mov rax, int98
	mov rcx, 1568
	call make_entry
	mov rax, int99
	mov rcx, 1584
	call make_entry
	mov rax, int100
	mov rcx, 1600
	call make_entry
	mov rax, int101
	mov rcx, 1616
	call make_entry
	mov rax, int102
	mov rcx, 1632
	call make_entry
	mov rax, int103
	mov rcx, 1648
	call make_entry
	mov rax, int104
	mov rcx, 1664
	call make_entry
	mov rax, int105
	mov rcx, 1680
	call make_entry
	mov rax, int106
	mov rcx, 1696
	call make_entry
	mov rax, int107
	mov rcx, 1712
	call make_entry
	mov rax, int108
	mov rcx, 1728
	call make_entry
	mov rax, int109
	mov rcx, 1744
	call make_entry
	mov rax, int110
	mov rcx, 1760
	call make_entry
	mov rax, int111
	mov rcx, 1776
	call make_entry
	mov rax, int112
	mov rcx, 1792
	call make_entry
	mov rax, int113
	mov rcx, 1808
	call make_entry
	mov rax, int114
	mov rcx, 1824
	call make_entry
	mov rax, int115
	mov rcx, 1840
	call make_entry
	mov rax, int116
	mov rcx, 1856
	call make_entry
	mov rax, int117
	mov rcx, 1872
	call make_entry
	mov rax, int118
	mov rcx, 1888
	call make_entry
	mov rax, int119
	mov rcx, 1904
	call make_entry
	mov rax, int120
	mov rcx, 1920
	call make_entry
	mov rax, int121
	mov rcx, 1936
	call make_entry
	mov rax, int122
	mov rcx, 1952
	call make_entry
	mov rax, int123
	mov rcx, 1968
	call make_entry
	mov rax, int124
	mov rcx, 1984
	call make_entry
	mov rax, int125
	mov rcx, 2000
	call make_entry
	mov rax, int126
	mov rcx, 2016
	call make_entry
	mov rax, int127
	mov rcx, 2032
	call make_entry
	mov rax, int128
	mov rcx, 2048
	call make_entry
	mov rax, int129
	mov rcx, 2064
	call make_entry
	mov rax, int130
	mov rcx, 2080
	call make_entry
	mov rax, int131
	mov rcx, 2096
	call make_entry
	mov rax, int132
	mov rcx, 2112
	call make_entry
	mov rax, int133
	mov rcx, 2128
	call make_entry
	mov rax, int134
	mov rcx, 2144
	call make_entry
	mov rax, int135
	mov rcx, 2160
	call make_entry
	mov rax, int136
	mov rcx, 2176
	call make_entry
	mov rax, int137
	mov rcx, 2192
	call make_entry
	mov rax, int138
	mov rcx, 2208
	call make_entry
	mov rax, int139
	mov rcx, 2224
	call make_entry
	mov rax, int140
	mov rcx, 2240
	call make_entry
	mov rax, int141
	mov rcx, 2256
	call make_entry
	mov rax, int142
	mov rcx, 2272
	call make_entry
	mov rax, int143
	mov rcx, 2288
	call make_entry
	mov rax, int144
	mov rcx, 2304
	call make_entry
	mov rax, int145
	mov rcx, 2320
	call make_entry
	mov rax, int146
	mov rcx, 2336
	call make_entry
	mov rax, int147
	mov rcx, 2352
	call make_entry
	mov rax, int148
	mov rcx, 2368
	call make_entry
	mov rax, int149
	mov rcx, 2384
	call make_entry
	mov rax, int150
	mov rcx, 2400
	call make_entry
	mov rax, int151
	mov rcx, 2416
	call make_entry
	mov rax, int152
	mov rcx, 2432
	call make_entry
	mov rax, int153
	mov rcx, 2448
	call make_entry
	mov rax, int154
	mov rcx, 2464
	call make_entry
	mov rax, int155
	mov rcx, 2480
	call make_entry
	mov rax, int156
	mov rcx, 2496
	call make_entry
	mov rax, int157
	mov rcx, 2512
	call make_entry
	mov rax, int158
	mov rcx, 2528
	call make_entry
	mov rax, int159
	mov rcx, 2544
	call make_entry
	mov rax, int160
	mov rcx, 2560
	call make_entry
	mov rax, int161
	mov rcx, 2576
	call make_entry
	mov rax, int162
	mov rcx, 2592
	call make_entry
	mov rax, int163
	mov rcx, 2608
	call make_entry
	mov rax, int164
	mov rcx, 2624
	call make_entry
	mov rax, int165
	mov rcx, 2640
	call make_entry
	mov rax, int166
	mov rcx, 2656
	call make_entry
	mov rax, int167
	mov rcx, 2672
	call make_entry
	mov rax, int168
	mov rcx, 2688
	call make_entry
	mov rax, int169
	mov rcx, 2704
	call make_entry
	mov rax, int170
	mov rcx, 2720
	call make_entry
	mov rax, int171
	mov rcx, 2736
	call make_entry
	mov rax, int172
	mov rcx, 2752
	call make_entry
	mov rax, int173
	mov rcx, 2768
	call make_entry
	mov rax, int174
	mov rcx, 2784
	call make_entry
	mov rax, int175
	mov rcx, 2800
	call make_entry
	mov rax, int176
	mov rcx, 2816
	call make_entry
	mov rax, int177
	mov rcx, 2832
	call make_entry
	mov rax, int178
	mov rcx, 2848
	call make_entry
	mov rax, int179
	mov rcx, 2864
	call make_entry
	mov rax, int180
	mov rcx, 2880
	call make_entry
	mov rax, int181
	mov rcx, 2896
	call make_entry
	mov rax, int182
	mov rcx, 2912
	call make_entry
	mov rax, int183
	mov rcx, 2928
	call make_entry
	mov rax, int184
	mov rcx, 2944
	call make_entry
	mov rax, int185
	mov rcx, 2960
	call make_entry
	mov rax, int186
	mov rcx, 2976
	call make_entry
	mov rax, int187
	mov rcx, 2992
	call make_entry
	mov rax, int188
	mov rcx, 3008
	call make_entry
	mov rax, int189
	mov rcx, 3024
	call make_entry
	mov rax, int190
	mov rcx, 3040
	call make_entry
	mov rax, int191
	mov rcx, 3056
	call make_entry
	mov rax, int192
	mov rcx, 3072
	call make_entry
	mov rax, int193
	mov rcx, 3088
	call make_entry
	mov rax, int194
	mov rcx, 3104
	call make_entry
	mov rax, int195
	mov rcx, 3120
	call make_entry
	mov rax, int196
	mov rcx, 3136
	call make_entry
	mov rax, int197
	mov rcx, 3152
	call make_entry
	mov rax, int198
	mov rcx, 3168
	call make_entry
	mov rax, int199
	mov rcx, 3184
	call make_entry
	mov rax, int200
	mov rcx, 3200
	call make_entry
	mov rax, int201
	mov rcx, 3216
	call make_entry
	mov rax, int202
	mov rcx, 3232
	call make_entry
	mov rax, int203
	mov rcx, 3248
	call make_entry
	mov rax, int204
	mov rcx, 3264
	call make_entry
	mov rax, int205
	mov rcx, 3280
	call make_entry
	mov rax, int206
	mov rcx, 3296
	call make_entry
	mov rax, int207
	mov rcx, 3312
	call make_entry
	mov rax, int208
	mov rcx, 3328
	call make_entry
	mov rax, int209
	mov rcx, 3344
	call make_entry
	mov rax, int210
	mov rcx, 3360
	call make_entry
	mov rax, int211
	mov rcx, 3376
	call make_entry
	mov rax, int212
	mov rcx, 3392
	call make_entry
	mov rax, int213
	mov rcx, 3408
	call make_entry
	mov rax, int214
	mov rcx, 3424
	call make_entry
	mov rax, int215
	mov rcx, 3440
	call make_entry
	mov rax, int216
	mov rcx, 3456
	call make_entry
	mov rax, int217
	mov rcx, 3472
	call make_entry
	mov rax, int218
	mov rcx, 3488
	call make_entry
	mov rax, int219
	mov rcx, 3504
	call make_entry
	mov rax, int220
	mov rcx, 3520
	call make_entry
	mov rax, int221
	mov rcx, 3536
	call make_entry
	mov rax, int222
	mov rcx, 3552
	call make_entry
	mov rax, int223
	mov rcx, 3568
	call make_entry
	mov rax, int224
	mov rcx, 3584
	call make_entry
	mov rax, int225
	mov rcx, 3600
	call make_entry
	mov rax, int226
	mov rcx, 3616
	call make_entry
	mov rax, int227
	mov rcx, 3632
	call make_entry
	mov rax, int228
	mov rcx, 3648
	call make_entry
	mov rax, int229
	mov rcx, 3664
	call make_entry
	mov rax, int230
	mov rcx, 3680
	call make_entry
	mov rax, int231
	mov rcx, 3696
	call make_entry
	mov rax, int232
	mov rcx, 3712
	call make_entry
	mov rax, int233
	mov rcx, 3728
	call make_entry
	mov rax, int234
	mov rcx, 3744
	call make_entry
	mov rax, int235
	mov rcx, 3760
	call make_entry
	mov rax, int236
	mov rcx, 3776
	call make_entry
	mov rax, int237
	mov rcx, 3792
	call make_entry
	mov rax, int238
	mov rcx, 3808
	call make_entry
	mov rax, int239
	mov rcx, 3824
	call make_entry
	mov rax, int240
	mov rcx, 3840
	call make_entry
	mov rax, int241
	mov rcx, 3856
	call make_entry
	mov rax, int242
	mov rcx, 3872
	call make_entry
	mov rax, int243
	mov rcx, 3888
	call make_entry
	mov rax, int244
	mov rcx, 3904
	call make_entry
	mov rax, int245
	mov rcx, 3920
	call make_entry
	mov rax, int246
	mov rcx, 3936
	call make_entry
	mov rax, int247
	mov rcx, 3952
	call make_entry
	mov rax, int248
	mov rcx, 3968
	call make_entry
	mov rax, int249
	mov rcx, 3984
	call make_entry
	mov rax, int250
	mov rcx, 4000
	call make_entry
	mov rax, int251
	mov rcx, 4016
	call make_entry
	mov rax, int252
	mov rcx, 4032
	call make_entry
	mov rax, int253
	mov rcx, 4048
	call make_entry
	mov rax, int254
	mov rcx, 4064
	call make_entry
	mov rax, int255
	mov rcx, 4080
	call make_entry
load_IDT:
    cli
    lidt [IDTP]
fix_cs:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, stack_top
    mov rbp, stack_top
    sti
    jmp interrupts_enabled


