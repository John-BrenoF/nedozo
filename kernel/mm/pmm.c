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

/* Macros para manipulação de bits no array */
#define BIT_SET(a,b)   ((a)[(b)/32] |=  (1 << ((b)%32)))
#define BIT_CLEAR(a,b) ((a)[(b)/32] &= ~(1 << ((b)%32)))
#define BIT_TEST(a,b)  ((a)[(b)/32] &   (1 << ((b)%32)))

void pmm_init(uint32_t mem_size_kb) {
    total_pages = (mem_size_kb * 1024) / PAGE_SIZE;
    free_pages  = total_pages;

    /* Inicialmente marca tudo como livre (zero) */
    memset(bitmap, 0, total_pages / 8);

    /* Protege a memória baixa e a região onde o kernel foi carregado (0-2MB) */
    for (uint32_t i = 0; i < (0x200000 / PAGE_SIZE); i++) {
        BIT_SET(bitmap, i);
        free_pages--;
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
