# Kernel Monolítico Simples

Kernel x86 32-bit com gerenciamento básico de memória.

## Estrutura
```
boot/               Bootloader (NASM, GRUB config)
kernel/
  arch/x86/         GDT, IDT (C + NASM)
  mm/               PMM, VMM, Heap
  drivers/vga/      Driver de texto VGA
  lib/              string.h mínimo
  kernel.c          Ponto de entrada em C
include/            Todos os headers
scripts/linker.ld   Layout de memória
Makefile            Build completo
```

## Requisitos
```bash
sudo apt install gcc nasm binutils qemu-system-x86
```

## Build & execução
```bash
make        # compila
make run    # roda no QEMU
make clean  # limpa build
```
