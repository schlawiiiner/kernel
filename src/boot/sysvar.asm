global tm
global fb
global bis
global mi

section .sysvar
; textmode
tm:
    dq 0        ; width
    dq 0        ; height
    dq 0        ; x_position
    dq 0        ; y_position
    dq 0        ; foreground
    dq 0        ; background
; frambuffer
fb:
    dq 0
    dq 0
    dq 0
    dq 0
; boot information
bis:
    dq 0        ; present flags
    dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

; memory information structure
mi:
    dq 0        ; memory size

