/* ============================================================
   kernel/mm/pmm.c — Physical Memory Manager (bitmap)
   Cada bit representa uma página de 4 KB.
   bit = 0 → livre | bit = 1 → ocupada
   ============================================================ */
#include <stdint.h>
#include <stddef.h>
#include <mm/pmm.h>
#include <string.h>

/* O bitmap é um array de inteiros tratando bits individuais */
static uint32_t *bitmap     = (uint32_t *)PMM_BITMAP_ADDR;
static uint32_t  total_pages = 0;
static uint32_t  free_pages  = 0;

/* Estruturas simplificadas do Multiboot para leitura do mapa de memória */
typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot_info_t;

/* Macros para manipulação de bits no array */
#define BIT_SET(a,b)   ((a)[(b)/32] |=  (1 << ((b)%32)))
#define BIT_CLEAR(a,b) ((a)[(b)/32] &= ~(1 << ((b)%32)))
#define BIT_TEST(a,b)  ((a)[(b)/32] &   (1 << ((b)%32)))

void pmm_init(void *mboot_ptr) {
    multiboot_info_t *mbi = (multiboot_info_t *)mboot_ptr;
    
    /* Assume 32MB se o bootloader não informar, ou calcula pela info do Multiboot */
    uint32_t mem_kb = mbi->mem_lower + mbi->mem_upper;
    total_pages = (mem_kb * 1024) / PAGE_SIZE;
    free_pages  = 0; /* Começamos com zero e vamos liberando as regiões RAM */

    /* Marca tudo como ocupado inicialmente (segurança estilo macOS) */
    memset(bitmap, 0xFF, total_pages / 8);

    /* Varre o mapa de memória para liberar apenas o que é RAM utilizável */
    mmap_entry_t *mmap = (mmap_entry_t *)mbi->mmap_addr;
    while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
        if (mmap->type == 1) { // 1 = RAM disponível
            for (uint64_t addr = mmap->addr; addr < mmap->addr + mmap->len; addr += PAGE_SIZE) {
                uint32_t page = addr / PAGE_SIZE;
                if (page < total_pages) {
                    BIT_CLEAR(bitmap, page);
                    free_pages++;
                }
            }
        }
        mmap = (mmap_entry_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    /* Re-protege os primeiros 2MB (Kernel + BIOS + Bitmap) */
    for (uint32_t i = 0; i < (0x200000 / PAGE_SIZE); i++) {
        if (!BIT_TEST(bitmap, i)) {
            BIT_SET(bitmap, i);
            free_pages--;
        }
    }
}

uint32_t pmm_alloc_page(void) {
    /* Busca linear simples (First-fit) no bitmap */
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!BIT_TEST(bitmap, i)) {
            BIT_SET(bitmap, i);
            free_pages--;
            return i * PAGE_SIZE;
        }
    }
    return 0; /* sem memória */
}

void pmm_free_page(uint32_t addr) {
    uint32_t page = addr / PAGE_SIZE;
    BIT_CLEAR(bitmap, page);
    free_pages++;
}

uint32_t pmm_free_pages_count(void) {
    return free_pages;
}

uint32_t pmm_total_pages_count(void) {
    return total_pages;
}
