; boot/boot.s — минимальный 64-bit загрузчик с Multiboot2

section .multiboot_header
align 8
header_start:
    dd 0xE85250D6                ; magic
    dd 0                         ; architecture (0 = i386)
    dd header_end - header_start ; header length
    dd -(0xE85250D6 + 0 + (header_end - header_start)) ; checksum
header_end:

section .text
global _start
extern kernel_main

_start:
    cli                     ; выключаем прерывания
    mov rsp, stack_top      ; указываем стек
    call kernel_main        ; вызываем ядро
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384              ; 16 KB стек
stack_top:

