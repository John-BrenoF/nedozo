/* ============================================================
   kernel/arch/x86/idt.c — Implementação da IDT
   ============================================================ */
#include <stdint.h>
#include <stddef.h>
#include <arch/idt.h>
#include <string.h>

/* A x86 suporta 256 vetores de interrupção */
#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idt_ptr;

extern void idt_flush(uint32_t); /* Definido em idt_flush.asm */

/* Preenche os campos de uma entrada da IDT */
void idt_set_gate(uint8_t n, uint32_t base,
                  uint16_t sel, uint8_t flags) {
    idt[n].base_low  = base & 0xFFFF;
    idt[n].base_high = (base >> 16) & 0xFFFF;
    idt[n].selector  = sel;
    idt[n].always0   = 0;
    idt[n].flags     = flags;
}

void idt_init(void) {
    /* Configura o registro IDTR */
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    /* Limpa a tabela inicialmente */
    memset(&idt, 0, sizeof(idt));

    /* No futuro, registraríamos ISRs (Interrupt Service Routines) aqui */

    /* Carrega o registro IDTR na CPU */
    idt_flush((uint32_t)&idt_ptr);
    
    /* __asm__ volatile("sti"); // Habilitar interrupções se necessário */
}
