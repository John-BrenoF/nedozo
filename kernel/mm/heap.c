/* ============================================================
   kernel/mm/heap.c — Zone Allocator (Estilo Mach/macOS)
   ============================================================ */
#include <mm/heap.h>
#include <mm/pmm.h>
#include <drivers/vga.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Definição de uma Zona (Slab) */
typedef struct {
    size_t   obj_size;
    void    *free_list;
    uint32_t num_free;
} zone_t;

/* Criamos zonas para os tamanhos mais usados no kernel */
static zone_t zones[4];

void heap_init(void *start, size_t size) {
    (void)start; (void)size;
    zones[0].obj_size = 32;
    zones[1].obj_size = 64;
    zones[2].obj_size = 128;
    zones[3].obj_size = 256;
    for(int i=0; i<4; i++) {
        zones[i].free_list = 0;
        zones[i].num_free = 0;
    }
}

void heap_dump_stats(void) {
    vga_set_color(VGA_CYAN, VGA_BLACK);
    vga_puts("\n--- Heap Zone Status ---\n");
    for (int i = 0; i < 4; i++) {
        kprintf("  Zone %u bytes: %u free objects\n", (uint32_t)zones[i].obj_size, zones[i].num_free);
    }
    vga_set_color(VGA_WHITE, VGA_BLACK);
}

/* Função para "alimentar" uma zona com uma nova página do PMM */
static void zone_refill(zone_t *z) {
    uint32_t page = pmm_alloc_page();
    if (!page) return;

    /* No estilo Mach, precisamos de metadados para saber o tamanho no kfree.
       Ajustamos o num_objs para caber o ponteiro de metadados. */
    uint32_t real_obj_size = z->obj_size + sizeof(size_t);
    uint32_t num_objs = PAGE_SIZE / real_obj_size;

    for (uint32_t i = 0; i < num_objs; i++) {
        void *obj = (void *)(page + (i * real_obj_size));
        /* O próprio objeto guarda o ponteiro para o próximo enquanto livre */
        *(void **)obj = z->free_list;
        z->free_list = obj;
        z->num_free++;
    }
}

void *kmalloc(size_t size) {
    for (int i = 0; i < 4; i++) {
        if (size <= zones[i].obj_size) {
            if (zones[i].num_free == 0) zone_refill(&zones[i]);
            if (zones[i].num_free == 0) return 0;

            /* Retira da lista de livres */
            size_t *ptr = (size_t *)zones[i].free_list;
            zones[i].free_list = *(void **)ptr;
            zones[i].num_free--;

            /* Armazena o tamanho original da zona para o kfree saber onde devolver */
            *ptr = zones[i].obj_size;

            /* Retorna o ponteiro após o cabeçalho de tamanho */
            return (void *)(ptr + 1);
        }
    }
    
    /* Para alocações grandes, o Mach usaria o VM System. 
       Aqui, apenas pegamos páginas inteiras do PMM. */
    uint32_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (pages_needed == 1) {
        size_t *ptr = (size_t *)pmm_alloc_page();
        if (!ptr) return 0;
        *ptr = PAGE_SIZE; /* Marca como uma alocação de página inteira */
        return (void *)(ptr + 1);
    }
    
    return 0; 
}

void kfree(void *ptr) {
    if (!ptr) return;

    /* Recupera o cabeçalho (tamanho da zona) */
    size_t *header = (size_t *)ptr - 1;
    size_t size = *header;

    /* Se for tamanho de página, devolve ao PMM */
    if (size == PAGE_SIZE) {
        pmm_free_page((uint32_t)header);
        return;
    }

    /* Caso contrário, procura a zona correspondente e insere na free_list */
    for (int i = 0; i < 4; i++) {
        if (size == zones[i].obj_size) {
            *(void **)header = zones[i].free_list;
            zones[i].free_list = (void *)header;
            zones[i].num_free++;
            return;
        }
    }
}
