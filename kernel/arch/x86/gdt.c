/* ============================================================
   kernel/arch/x86/gdt.c — Implementação da Global Descriptor Table
   ============================================================ */
#include <stdint.h>
#include <stddef.h>
#include <arch/gdt.h>

/* Definimos 3 entradas: Nula, Código Kernel, Dados Kernel */
#define GDT_ENTRIES 3

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t   gdt_ptr;

/* Declarado externamente em gdt_flush.asm */
extern void gdt_flush(uint32_t);

/* Decompõe os valores de base e limite nos campos bit a bit da GDT */
static void gdt_set_gate(int n, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t gran) {
    gdt[n].base_low    = base & 0xFFFF;
    gdt[n].base_middle = (base >> 16) & 0xFF;
    gdt[n].base_high   = (base >> 24) & 0xFF;

    gdt[n].limit_low   = limit & 0xFFFF;
    gdt[n].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[n].access      = access;
}

void gdt_init(void) {
    /* Configura o registro GDTR */
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    /* Flat setup: base 0, limite 4GB */
    gdt_set_gate(0, 0, 0,          0,    0);      /* Segmento Nulo (obrigatório) */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  /* Código Kernel (Ring 0) */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  /* Dados Kernel (Ring 0) */

    /* Aplica as mudanças nos registros da CPU */
    gdt_flush((uint32_t)&gdt_ptr);
}
