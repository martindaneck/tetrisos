loop:
    jmp loop 

times 510-($-$$) db 0 ; pad 0s to the 510th byte

dw 0xaa55 ; boot sector magic number