/* kernel/kernel.c — Inicialização e Loop Principal*/

#include <stdint.h>
#include <drivers/vga.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>

/* Endereço e tamanho do heap do kernel (ajuste conforme necessário) */
#define HEAP_START  0x500000    /* 5 MB */
#define HEAP_SIZE   (1024*1024) /* 1 MB  */

void display_mem_info() {
    uint32_t free = pmm_free_pages_count();
    uint32_t total = pmm_total_pages_count();
    uint32_t used = total - free;

    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kprintf("\n[ RAM Status: %u/%u pages used (%u KB free) ]\n", 
            used, total, free * 4);
    heap_dump_stats();
}

void kernel_main(uint32_t magic, void *mboot_info) {
    __asm__ volatile ("cli"); // Segurança extra: garante interrupções desligadas

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
    pmm_init(mboot_info);           /* Detecção dinâmica estilo macOS */
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
    vga_puts("\n--- Teste de Alocacao (Zone Allocator) ---\n");
    void *p1 = kmalloc(128);
    void *p2 = kmalloc(256);
    void *p3 = kmalloc(128);
    
    kprintf("  p1 (128b) = 0x%x | p2 (256b) = 0x%x\n", (uint32_t)p1, (uint32_t)p2);
    kprintf("  p3 (128b) = 0x%x\n", (uint32_t)p3);

    display_mem_info();  /* Mostra o status em tempo real com as zonas ocupadas */

    vga_puts("\n--- Liberando memoria (kfree) ---\n");
    kfree(p1);
    kfree(p2);
    
    display_mem_info();  /* Mostra as zonas sendo liberadas */

    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("\nKernel inicializado. Sistema ocioso.\n");

    /* Loop de espera */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
