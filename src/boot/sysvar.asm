section .sysvar
    ; textmode 0x100030
    dw 0        ; width
    dw 0        ; height
    dw 0        ; x_position
    dw 0        ; y_position
    ; framebuffer 0x100038
    dq 0
    dw 0
    dw 0
    dw 0
