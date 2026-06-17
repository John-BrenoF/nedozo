/* ============================================================
   include/arch/gdt.h — Estruturas da Global Descriptor Table
   ============================================================ */
#ifndef ARCH_GDT_H
#define ARCH_GDT_H

#include <stdint.h>

/* Cada entrada da GDT tem 8 bytes e define permissões de um segmento */
typedef struct {
    uint16_t limit_low;    /* Parte baixa do limite (16 bits) */
    uint16_t base_low;     /* Parte baixa da base (16 bits) */
    uint8_t  base_middle;  /* Parte média da base (8 bits) */
    uint8_t  access;       /* Flags de acesso (P, DPL, S, Type) */
    uint8_t  granularity;  /* Granularidade (G, D/B, L, AVL) + limit_high */
    uint8_t  base_high;    /* Parte alta da base (8 bits) */
} __attribute__((packed)) gdt_entry_t;

/* Registro GDTR: Aponta para a tabela GDT */
typedef struct {
    uint16_t limit;        /* Tamanho da tabela - 1 */
    uint32_t base;         /* Endereço linear da tabela */
} __attribute__((packed)) gdt_ptr_t;

/* Inicializa a GDT padrão do sistema */
void gdt_init(void);

#endif /* ARCH_GDT_H */
