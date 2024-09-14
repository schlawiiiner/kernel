global tm
global fb
global bi

section .sysvar
tm:
    dq 0        ; width
    dq 0        ; height
    dq 0        ; x_position
    dq 0        ; y_position
    dq 0        ; foreground
    dq 0        ; background

fb:
    dq 0
    dq 0
    dq 0
    dq 0

bi:
    dq 0        ; present flags
    dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

