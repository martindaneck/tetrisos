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
# some random stuff
.long 0 
.long 0 
.long 0
.long 0
.long 0
# video
.long 0 # mode_type - video mode
.long 1024 # width
.long 768 # height
.long 32 # depth

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
    pushl %ebx # push the pointer to the multiboot structure

    /* call the kernel main function */
    call kernel_main

    /* hang */
    cli
1:  hlt
    jmp 1b
