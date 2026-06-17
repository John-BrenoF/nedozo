; ============================================================
; kernel/arch/x86/gdt_flush.asm — Recarrega registros de segmento
; ============================================================
global gdt_flush

gdt_flush:
    mov eax, [esp+4]    ; Pega o ponteiro da GDT passado como argumento
    lgdt [eax]          ; Carrega a nova GDT

    mov ax, 0x10        ; 0x10 é o deslocamento do seletor de dados (Kernel Data)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush     ; Far jump para 0x08 (Kernel Code) para recarregar o registro CS
.flush:
    ret                 ; Retorna para o chamador em C
