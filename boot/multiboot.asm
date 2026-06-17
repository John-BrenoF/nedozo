; ============================================================
; boot/multiboot.asm — Ponto de entrada e cabeçalho Multiboot
; Sintaxe NASM, x86 32-bit (i386)
; ============================================================

; Definições do cabeçalho Multiboot v1
MBALIGN     equ 1 << 0              ; Alinhamento de módulos em páginas
MEMINFO     equ 1 << 1              ; Fornecer mapa de memória
FLAGS       equ MBALIGN | MEMINFO   ; Flags combinadas
MAGIC       equ 0x1BADB002          ; Número mágico para o bootloader (GRUB)
CHECKSUM    equ -(MAGIC + FLAGS)    ; Checksum para validar o cabeçalho

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
; Reserva espaço para a pilha (stack) do kernel
section .bss
align 16
stack_bottom:
    resb 16384          ; Aloca 16 KB
stack_top:

; Código de inicialização
section .text
global _start
extern kernel_main      ; Ponto de entrada definido em C
extern _bss_start       ; Definido no linker script
extern _bss_end         ; Definido no linker script

_start:
    cli
    cld                 ; Garante que operações de string (rep) incrementem o endereço

    ; Salva registros do Multiboot antes de limpar o BSS
    push ebx
    push eax

    mov edi, _bss_start
    xor eax, eax
    mov ecx, _bss_end
    sub ecx, edi
    rep stosb

    ; Restaura registros e configura pilha
    pop eax
    pop ebx
    xor  ebp, ebp       ; Reseta o frame pointer (boa prática)
    mov  esp, stack_top
    push ebx            ; ponteiro para info Multiboot (mmap)
    push eax            ; magic number do Multiboot
    call kernel_main

    ; Desabilita interrupções e trava o processador se o kernel retornar
    cli
.hang:
    hlt                 ; Para a execução até a próxima interrupção (que não virá)
    jmp .hang           ; Loop de segurança