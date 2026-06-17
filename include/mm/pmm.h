/* ============================================================
   include/mm/pmm.h — Physical Memory Manager
   ============================================================ */
#ifndef MM_PMM_H
#define MM_PMM_H

#include <stdint.h>
#include <stddef.h>

/* Tamanho padrão de página x86 */
#define PAGE_SIZE       4096 

/* Endereço fixo para o bitmap de gerenciamento (área livre na memória baixa) */
#define PMM_BITMAP_ADDR 0x20000   

/* Inicializa o gerenciador com a memória disponível em KB */
void pmm_init(uint32_t mem_size_kb);

uint32_t pmm_alloc_page(void);      /* Aloca uma página física e retorna seu endereço */
void     pmm_free_page(uint32_t addr); /* Libera uma página dado seu endereço */

uint32_t pmm_free_pages_count(void); /* Retorna o número de páginas livres */

#endif /* MM_PMM_H */
