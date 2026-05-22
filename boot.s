/* multiboot header constants */
.set ALIGN,    1<<0             
.set MEMINFO,  1<<1
.set VIDEO,    1<<2             
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS)

/* multiboot header */
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.skip 20 # offset to video stuff
.long 0 # mode_type - video mode
.long 320 # width
.long 200 # height
.long 8 # depth

/* stack */
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/* kernel entry point */
.section .text
.global _start
.type _start, @function
_start:
    /* set up the stack */
    mov $stack_top, %esp

    /* potentially do extra stuff */
    

    /* call the kernel main function */
    call kernel_main

    /* hang */
    cli
1:  hlt
    jmp 1b
