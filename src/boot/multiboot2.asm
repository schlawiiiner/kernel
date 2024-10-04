%if SCREEN_X
%else
    %define SCREEN_X        1920
%endif
%if SCREEN_Y
%else
    %define SCREEN_Y        1080
%endif


section .multiboot2
header_start:
align 8
    dd 0xe85250d6                               ; magic
    dd 0                                        ; arcitecture: 32-bit (protected) mode of i386
    dd header_end - header_start                ; header length
    dd -0xe85250d6 - header_end + header_start  ; checksum

align 8
    ; framebuffer-tag
    dw 5
    dw 0
    dd 20
    dd SCREEN_X, SCREEN_Y, 32

align 8
    ; end-tag
    dw 0
    dw 0
    dd 8

header_end: