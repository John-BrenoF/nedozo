/* ============================================================
   scripts/linker.ld — Layout do binário ELF do kernel
   Carregado em 0x100000 (1 MB) para evitar áreas de BIOS/Video
   ============================================================ */
ENTRY(_start)

SECTIONS {
    /* Descarta seções que podem interferir com o cabeçalho Multiboot no ELF */
    /DISCARD/ : {
        *(.note.gnu.property)
        *(.comment)
    }

    /* O kernel começa a ser carregado em 1 MB físico */
    . = 0x100000;

    .text ALIGN(4096) : {
        *(.multiboot)  /* O cabeçalho Multiboot DEVE estar no início do arquivo */
        *(.text*)
    }

    .rodata ALIGN(4096) : {
        *(.rodata*)
    }

    .data ALIGN(4096) : {
        *(.data*)
    }

    .bss ALIGN(4096) : {
        _bss_start = .;
        *(COMMON)
        *(.bss*)
        _bss_end = .;
    }
}
