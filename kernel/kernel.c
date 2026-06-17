/* ============================================================
   kernel/kernel.c — Inicialização e Loop Principal
   ============================================================ */
#include <stdint.h>
#include "include/drivers/vga.h"
#include "include/arch/gdt.h"
#include "include/arch/idt.h"
#include "include/mm/pmm.h"
#include "include/mm/vmm.h"
#include "include/mm/heap.h"

/* Endereço e tamanho do heap do kernel (ajuste conforme necessário) */
#define HEAP_START  0x500000    /* 5 MB */
#define HEAP_SIZE   (1024*1024) /* 1 MB  */

void kernel_main(uint32_t magic, void *mboot_info) {
    (void)magic;
    (void)mboot_info;

    /* 1. Driver de vídeo — precisa ser o primeiro */
    vga_init();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("[ OK ] VGA inicializado\n");

    /* 2. Tabelas de descritores */
    gdt_init();
    vga_puts("[ OK ] GDT carregada\n");

    idt_init();
    vga_puts("[ OK ] IDT carregada\n");

    /* 3. Gerenciamento de memória física */
    pmm_init(32 * 1024);            /* 32 MB de RAM */
    kprintf("[ OK ] PMM inicializado. Paginas livres: %u\n", pmm_free_pages_count());
    vga_puts("[ OK ] PMM inicializado\n");

    /* 4. Paginação / memória virtual */
    vmm_init();
    vga_puts("[ OK ] Paginacao ativa\n");

    /* 5. Heap do kernel */
    heap_init((void *)HEAP_START, HEAP_SIZE);
    vga_puts("[ OK ] Heap inicializado\n");

    /* --- Testes básicos de alocação --- */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("\n--- Teste de kmalloc ---\n");
    void *p1 = kmalloc(128);
    void *p2 = kmalloc(256);
    kprintf("  p1 = 0x%x\n", (uint32_t)p1);
    kprintf("  p2 = 0x%x\n", (uint32_t)p2);
    kfree(p1);
    kfree(p2);
    vga_puts("  kfree: OK\n");

    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("\nKernel inicializado. Sistema ocioso.\n");

    /* Loop de espera */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
