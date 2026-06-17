/* ============================================================
   kernel/mm/vmm.c — Virtual Memory Manager
   Configura diretório e tabelas de página (Paging de 2 níveis).
   Endereço: [31..22] Directory Index | [21..12] Table Index | [11..0] Offset
   ============================================================ */
#include <stdint.h>
#include <stddef.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <string.h>

/* Tabelas de 1024 entradas (cada entrada 32 bits = 4 bytes) */
#define PD_ENTRIES 1024
#define PT_ENTRIES 1024

/* Page Directory principal do kernel, alinhado em fronteira de página */
static uint32_t page_directory[PD_ENTRIES] __attribute__((aligned(4096)));

void vmm_init(void) {
    memset(page_directory, 0, sizeof(page_directory));

    /* Mapeamento Identidade (Virtual = Físico) dos primeiros 16 MB */
    /* Aumentamos para 16MB para garantir que as tabelas de página alocadas pelo PMM caibam aqui */
    for (uint32_t j = 0; j < 4; j++) {
        uint32_t *pt = (uint32_t *)pmm_alloc_page();
        memset(pt, 0, PAGE_SIZE);
        for (uint32_t i = 0; i < PT_ENTRIES; i++) {
            pt[i] = ((j * 0x400000) + (i * PAGE_SIZE)) | PAGE_PRESENT | PAGE_WRITABLE;
        }
        page_directory[j] = (uint32_t)pt | PAGE_PRESENT | PAGE_WRITABLE;
    }


    /* Mapeamento Identidade (Virtual = Físico) dos primeiros 16 MB para segurança */
    for (uint32_t j = 1; j < 4; j++) {
        uint32_t *new_pt = (uint32_t *)pmm_alloc_page();
        memset(new_pt, 0, PAGE_SIZE);
        for (uint32_t i = 0; i < PT_ENTRIES; i++) {
            new_pt[i] = ((j * 0x400000) + (i * PAGE_SIZE)) | PAGE_PRESENT | PAGE_WRITABLE;
        }
        page_directory[j] = (uint32_t)new_pt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Ativação ultra-segura: usa EAX para CR3 e CR0 */
    __asm__ volatile (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80010000, %%eax\n"
        "mov %%eax, %%cr0\n"
        "jmp 1f\n"
        "1:\n"
        : : "r"(page_directory) : "eax", "memory"
    );
}
void vmm_map_page(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_idx = virt >> 22;           /* 10 bits superiores */
    uint32_t pt_idx = (virt >> 12) & 0x3FF; /* 10 bits intermediários */

    /* Se a Page Table não existir no diretório, aloca uma nova */
    if (!(page_directory[pd_idx] & PAGE_PRESENT)) {
        uint32_t *new_pt = (uint32_t *)pmm_alloc_page();
        memset(new_pt, 0, PAGE_SIZE);
        /* O endereço da PT deve estar alinhado a 4KB (os 12 bits baixos são flags) */
        page_directory[pd_idx] = (uint32_t)new_pt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    uint32_t *pt = (uint32_t *)(page_directory[pd_idx] & ~0xFFF);
    pt[pt_idx] = (phys & ~0xFFF) | (flags & 0xFFF) | PAGE_PRESENT;

    /* Invalida o TLB para esse endereço */
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

void vmm_unmap_page(uint32_t virt) {
    uint32_t pd_idx = virt >> 22;
    uint32_t pt_idx = (virt >> 12) & 0x3FF;

    if (!(page_directory[pd_idx] & PAGE_PRESENT)) return; /* Nada para desmapear */

    uint32_t *pt = (uint32_t *)(page_directory[pd_idx] & ~0xFFF);
    pt[pt_idx] = 0;

    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}
