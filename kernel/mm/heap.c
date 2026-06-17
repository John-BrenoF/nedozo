/* ============================================================
   kernel/mm/heap.c — Alocador de heap simples (first-fit)
   Cada bloco tem um cabeçalho com: tamanho + flag livre/usado.
   ============================================================ */
#include <mm/heap.h>
#include <stddef.h>
#include <stdint.h>

#define HEADER_SIZE sizeof(block_hdr_t)

static block_hdr_t *heap_head = 0;

void heap_init(void *start, size_t size) {
    heap_head       = (block_hdr_t *)start;
    heap_head->size = size - HEADER_SIZE;
    heap_head->free = 1;
    heap_head->next = 0;
}

/* Divide um bloco em dois se houver espaço suficiente sobrando */
static void split_block(block_hdr_t *blk, size_t size) {
    if (blk->size > size + HEADER_SIZE + 8) {
        /* Usamos uint8_t* para aritmética de ponteiros byte a byte */
        block_hdr_t *new_blk = (block_hdr_t *)((uint8_t *)blk + HEADER_SIZE + size);
        new_blk->size = blk->size - size - HEADER_SIZE;
        new_blk->free = 1;
        new_blk->next = blk->next;
        blk->size     = size;
        blk->next     = new_blk;
    }
}

void *kmalloc(size_t size) {
    /* Algoritmo First-fit: pega o primeiro que serve */
    block_hdr_t *cur = heap_head;
    while (cur) {
        if (cur->free && cur->size >= size) {
            split_block(cur, size);
            cur->free = 0;
            return (void *)((uint8_t *)cur + HEADER_SIZE);
        }
        cur = cur->next;
    }
    return 0; /* sem memória */
}

/* Mescla blocos livres adjacentes (Forward Coalescing) */
static void merge_free_blocks(void) {
    block_hdr_t *cur = heap_head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += HEADER_SIZE + cur->next->size;
            cur->next  = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_hdr_t *blk = (block_hdr_t *)((uint8_t *)ptr - HEADER_SIZE);
    blk->free = 1;
    merge_free_blocks();
}
