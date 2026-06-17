/* ============================================================
   include/mm/heap.h — Alocador Dinâmico do Kernel
   ============================================================ */
#ifndef MM_HEAP_H
#define MM_HEAP_H

#include <stddef.h>

/* Cabeçalho de cada bloco na lista ligada do heap */
typedef struct block_hdr {
    size_t            size;   /* Tamanho útil do bloco (dados) */
    int               free;   /* Flag: 1 se livre, 0 se ocupado */
    struct block_hdr *next;   /* Próximo bloco na memória */
} block_hdr_t;

/* Inicializa o heap no endereço especificado */
void  heap_init(void *start, size_t size);

/* Aloca 'size' bytes e retorna ponteiro para a área de dados */
void *kmalloc(size_t size);

/* Libera a memória previamente alocada */
void  kfree(void *ptr);

/* Exibe o status das zonas de memória */
void heap_dump_stats(void);

#endif /* MM_HEAP_H */
