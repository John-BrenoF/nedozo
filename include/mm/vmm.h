/* ============================================================
   include/mm/vmm.h — Virtual Memory Manager
   ============================================================ */
#ifndef MM_VMM_H
#define MM_VMM_H

#include <stdint.h>

/* Base virtual onde o kernel costuma residir (Higher Half) */
#define KERNEL_VIRTUAL_BASE 0xC0000000  /* 3 GB — espaço do kernel */

/* Atributos de página x86 */
#define PAGE_PRESENT  (1 << 0)  /* Página carregada na RAM */
#define PAGE_WRITABLE (1 << 1)  /* Permissão de escrita */
#define PAGE_USER     (1 << 2)  /* Acesso em modo usuário */

/* Inicializa paginação e mapeia o kernel */
void vmm_init(void);

/* Mapeia um endereço virtual para um físico com as flags dadas */
void vmm_map_page(uint32_t virt, uint32_t phys, uint32_t flags);
void vmm_unmap_page(uint32_t virt); /* Remove mapeamento virtual */

#endif /* MM_VMM_H */
