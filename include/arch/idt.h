/* ============================================================
   include/arch/idt.h — Estruturas da Interrupt Descriptor Table
   ============================================================ */
#ifndef ARCH_IDT_H
#define ARCH_IDT_H

#include <stdint.h>

/* Cada entrada da IDT define como a CPU deve tratar uma interrupção */
typedef struct {
    uint16_t base_low;  /* Parte baixa do endereço do handler */
    uint16_t selector;  /* Seletor de segmento de código (GDT) */
    uint8_t  always0;   /* Reservado, deve ser zero */
    uint8_t  flags;     /* Flags (Presente, DPL, Tipo de Gate) */
    uint16_t base_high; /* Parte alta do endereço do handler */
} __attribute__((packed)) idt_entry_t;

/* Registro IDTR: Aponta para a tabela IDT */
typedef struct {
    uint16_t limit;     /* Tamanho da tabela - 1 */
    uint32_t base;      /* Endereço linear da tabela */
} __attribute__((packed)) idt_ptr_t;

/* Inicializa a IDT com handlers vazios */
void idt_init(void);

#endif /* ARCH_IDT_H */
