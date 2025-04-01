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
extern intel_reserved
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
extern default_handler_func

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
	pushfq
%endmacro

%macro POP_ALL_REGS 0
	popfq
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

%define INTERRUPT_JUMP_TABLE_OFFSET		int1 - int0

section .interrupts
int0:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 0
	jmp intx

int1:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 1
	jmp intx

int2:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 2
	jmp intx

int3:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 3
	jmp intx

int4:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 4
	jmp intx

int5:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 5
	jmp intx

int6:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 6
	jmp intx

int7:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 7
	jmp intx

int8:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 8
	jmp intx

int9:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 9
	jmp intx

int10:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 10
	jmp intx

int11:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 11
	jmp intx

int12:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 12
	jmp intx

int13:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 13
	jmp intx

int14:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 14
	jmp intx

int15:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 15
	jmp intx

int16:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 16
	jmp intx

int17:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 17
	jmp intx

int18:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 18
	jmp intx

int19:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 19
	jmp intx

int20:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 20
	jmp intx

int21:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 21
	jmp intx

int22:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 22
	jmp intx

int23:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 23
	jmp intx

int24:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 24
	jmp intx

int25:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 25
	jmp intx

int26:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 26
	jmp intx

int27:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 27
	jmp intx

int28:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 28
	jmp intx

int29:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 29
	jmp intx

int30:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 30
	jmp intx

int31:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 31
	jmp intx

int32:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 32
	jmp intx

int33:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 33
	jmp intx

int34:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 34
	jmp intx

int35:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 35
	jmp intx

int36:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 36
	jmp intx

int37:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 37
	jmp intx

int38:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 38
	jmp intx

int39:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 39
	jmp intx

int40:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 40
	jmp intx

int41:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 41
	jmp intx

int42:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 42
	jmp intx

int43:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 43
	jmp intx

int44:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 44
	jmp intx

int45:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 45
	jmp intx

int46:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 46
	jmp intx

int47:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 47
	jmp intx

int48:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 48
	jmp intx

int49:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 49
	jmp intx

int50:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 50
	jmp intx

int51:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 51
	jmp intx

int52:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 52
	jmp intx

int53:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 53
	jmp intx

int54:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 54
	jmp intx

int55:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 55
	jmp intx

int56:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 56
	jmp intx

int57:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 57
	jmp intx

int58:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 58
	jmp intx

int59:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 59
	jmp intx

int60:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 60
	jmp intx

int61:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 61
	jmp intx

int62:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 62
	jmp intx

int63:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 63
	jmp intx

int64:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 64
	jmp intx

int65:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 65
	jmp intx

int66:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 66
	jmp intx

int67:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 67
	jmp intx

int68:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 68
	jmp intx

int69:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 69
	jmp intx

int70:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 70
	jmp intx

int71:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 71
	jmp intx

int72:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 72
	jmp intx

int73:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 73
	jmp intx

int74:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 74
	jmp intx

int75:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 75
	jmp intx

int76:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 76
	jmp intx

int77:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 77
	jmp intx

int78:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 78
	jmp intx

int79:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 79
	jmp intx

int80:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 80
	jmp intx

int81:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 81
	jmp intx

int82:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 82
	jmp intx

int83:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 83
	jmp intx

int84:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 84
	jmp intx

int85:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 85
	jmp intx

int86:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 86
	jmp intx

int87:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 87
	jmp intx

int88:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 88
	jmp intx

int89:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 89
	jmp intx

int90:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 90
	jmp intx

int91:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 91
	jmp intx

int92:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 92
	jmp intx

int93:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 93
	jmp intx

int94:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 94
	jmp intx

int95:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 95
	jmp intx

int96:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 96
	jmp intx

int97:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 97
	jmp intx

int98:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 98
	jmp intx

int99:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 99
	jmp intx

int100:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 100
	jmp intx

int101:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 101
	jmp intx

int102:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 102
	jmp intx

int103:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 103
	jmp intx

int104:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 104
	jmp intx

int105:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 105
	jmp intx

int106:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 106
	jmp intx

int107:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 107
	jmp intx

int108:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 108
	jmp intx

int109:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 109
	jmp intx

int110:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 110
	jmp intx

int111:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 111
	jmp intx

int112:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 112
	jmp intx

int113:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 113
	jmp intx

int114:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 114
	jmp intx

int115:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 115
	jmp intx

int116:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 116
	jmp intx

int117:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 117
	jmp intx

int118:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 118
	jmp intx

int119:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 119
	jmp intx

int120:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 120
	jmp intx

int121:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 121
	jmp intx

int122:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 122
	jmp intx

int123:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 123
	jmp intx

int124:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 124
	jmp intx

int125:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 125
	jmp intx

int126:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 126
	jmp intx

int127:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 127
	jmp intx

int128:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 128
	jmp intx

int129:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 129
	jmp intx

int130:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 130
	jmp intx

int131:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 131
	jmp intx

int132:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 132
	jmp intx

int133:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 133
	jmp intx

int134:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 134
	jmp intx

int135:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 135
	jmp intx

int136:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 136
	jmp intx

int137:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 137
	jmp intx

int138:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 138
	jmp intx

int139:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 139
	jmp intx

int140:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 140
	jmp intx

int141:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 141
	jmp intx

int142:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 142
	jmp intx

int143:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 143
	jmp intx

int144:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 144
	jmp intx

int145:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 145
	jmp intx

int146:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 146
	jmp intx

int147:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 147
	jmp intx

int148:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 148
	jmp intx

int149:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 149
	jmp intx

int150:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 150
	jmp intx

int151:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 151
	jmp intx

int152:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 152
	jmp intx

int153:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 153
	jmp intx

int154:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 154
	jmp intx

int155:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 155
	jmp intx

int156:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 156
	jmp intx

int157:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 157
	jmp intx

int158:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 158
	jmp intx

int159:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 159
	jmp intx

int160:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 160
	jmp intx

int161:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 161
	jmp intx

int162:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 162
	jmp intx

int163:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 163
	jmp intx

int164:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 164
	jmp intx

int165:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 165
	jmp intx

int166:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 166
	jmp intx

int167:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 167
	jmp intx

int168:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 168
	jmp intx

int169:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 169
	jmp intx

int170:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 170
	jmp intx

int171:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 171
	jmp intx

int172:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 172
	jmp intx

int173:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 173
	jmp intx

int174:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 174
	jmp intx

int175:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 175
	jmp intx

int176:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 176
	jmp intx

int177:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 177
	jmp intx

int178:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 178
	jmp intx

int179:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 179
	jmp intx

int180:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 180
	jmp intx

int181:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 181
	jmp intx

int182:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 182
	jmp intx

int183:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 183
	jmp intx

int184:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 184
	jmp intx

int185:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 185
	jmp intx

int186:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 186
	jmp intx

int187:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 187
	jmp intx

int188:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 188
	jmp intx

int189:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 189
	jmp intx

int190:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 190
	jmp intx

int191:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 191
	jmp intx

int192:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 192
	jmp intx

int193:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 193
	jmp intx

int194:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 194
	jmp intx

int195:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 195
	jmp intx

int196:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 196
	jmp intx

int197:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 197
	jmp intx

int198:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 198
	jmp intx

int199:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 199
	jmp intx

int200:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 200
	jmp intx

int201:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 201
	jmp intx

int202:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 202
	jmp intx

int203:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 203
	jmp intx

int204:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 204
	jmp intx

int205:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 205
	jmp intx

int206:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 206
	jmp intx

int207:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 207
	jmp intx

int208:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 208
	jmp intx

int209:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 209
	jmp intx

int210:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 210
	jmp intx

int211:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 211
	jmp intx

int212:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 212
	jmp intx

int213:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 213
	jmp intx

int214:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 214
	jmp intx

int215:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 215
	jmp intx

int216:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 216
	jmp intx

int217:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 217
	jmp intx

int218:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 218
	jmp intx

int219:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 219
	jmp intx

int220:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 220
	jmp intx

int221:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 221
	jmp intx

int222:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 222
	jmp intx

int223:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 223
	jmp intx

int224:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 224
	jmp intx

int225:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 225
	jmp intx

int226:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 226
	jmp intx

int227:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 227
	jmp intx

int228:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 228
	jmp intx

int229:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 229
	jmp intx

int230:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 230
	jmp intx

int231:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 231
	jmp intx

int232:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 232
	jmp intx

int233:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 233
	jmp intx

int234:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 234
	jmp intx

int235:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 235
	jmp intx

int236:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 236
	jmp intx

int237:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 237
	jmp intx

int238:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 238
	jmp intx

int239:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 239
	jmp intx

int240:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 240
	jmp intx

int241:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 241
	jmp intx

int242:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 242
	jmp intx

int243:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 243
	jmp intx

int244:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 244
	jmp intx

int245:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 245
	jmp intx

int246:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 246
	jmp intx

int247:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 247
	jmp intx

int248:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 248
	jmp intx

int249:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 249
	jmp intx

int250:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 250
	jmp intx

int251:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 251
	jmp intx

int252:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 252
	jmp intx

int253:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 253
	jmp intx

int254:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 254
	jmp intx

int255:
	cli
	cld
	PUSH_ALL_REGS
	mov rsi, 255
	jmp intx

intx:
	mov rdi, rsp
	mov rax, rsp
	and rax, 0xf
	cmp rax, 0
	jnz .align_stack
	mov rax, [irq_handlers+rsi*8]
    call rax
	jmp .end
.align_stack:
	sub rsp, 8
	mov rax, [irq_handlers+rsi*8]
	call rax
	add rsp, 8
.end:
	POP_ALL_REGS
	iretq

section .boot
exception_halt_main:
    cli
    hlt
    jmp exception_halt_main

; rax : address of handler function
; rbx : offset of IDT in bytes (e.g. for #DF 0x8*16)
make_entry:
    mov rdx, IDT
    add rdx, rbx
    mov word [rdx], ax				; offset [0 ... 15]
    mov word [rdx+2], CODE_SEG		; segment selector
    mov byte [rdx+4], 0x0			; IST (no interrupt stack table is beeing used)
    mov byte [rdx+5], 0xEF			; gate type, DPL, present (Trap gate, Ring 3, present) 
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
	xor rbx, rbx
	xor rcx, rcx
	mov r10, 16
	mov r11, INTERRUPT_JUMP_TABLE_OFFSET
	mov r12, int0
.loop:
	mov rax, rcx
	mul r11
	add rax, r12
	call make_entry
	inc rcx
	add rbx, r10
	cmp rcx, 256
	jl .loop
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

section .irq_table
irq_handlers:
	dq division_error
	dq debug
	dq non_maskable_interrupt
	dq breakpoint
	dq overflow
	dq bound_range_exceeded
	dq invalid_opcode
	dq device_not_available
	dq double_fault
	dq coprocessor_segment_overrun
	dq invalid_tss 
	dq segment_not_present
	dq stack_segment_fault
	dq general_protection_fault
	dq page_fault
	dq intel_reserved
	dq x87_floating_point_exception
	dq alignment_check
	dq machine_check
	dq simd_floating_point_exception
	dq virtualization_exception
	dq control_protection_exception
	dq intel_reserved
	dq intel_reserved
	dq intel_reserved
	dq intel_reserved
	dq intel_reserved
	dq intel_reserved
	dq hypervisor_injection_exception
	dq vmm_communication_exception
	dq security_exception
	dq intel_reserved
	times 224 dq default_handler_func
