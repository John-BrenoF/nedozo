; ============================================================
; kernel/arch/x86/idt_flush.asm — Carrega o registro IDTR
; ============================================================
global idt_flush

idt_flush:
    ; O GCC pode não alinhar corretamente estruturas passadas na stack
    ; por isso usamos uma rotina em Assembly puro.
    mov eax, [esp+4]    ; Endereço da estrutura idt_ptr
    lidt [eax]          ; Carrega a Interrupt Descriptor Table
    ret
