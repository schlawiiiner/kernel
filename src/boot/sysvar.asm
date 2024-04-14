section .sysvar
    ; textmode 0x100030
    dq 0        ; width
    dq 0        ; height
    dq 0        ; x_position
    dq 0        ; y_position
    dq 0        ; foreground
    dq 0        ; background

    ; framebuffer 0x100060
    dq 0
    dq 0
    dq 0
    dq 0

    ; boot-info tag structure 0x100080
    dq 0        ; present flags
    dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

