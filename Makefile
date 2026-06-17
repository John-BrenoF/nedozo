# ============================================================
#  Makefile — Kernel Monolítico
#  Ferramentas: gcc, nasm, ld, qemu-system-i386
# ============================================================

# -ffreestanding: O kernel não possui biblioteca padrão (libc)
# -nostdinc: Não usa headers do sistema hospedeiro
# -fno-pie/PIC: Desativa código independente de posição (essencial para kernels simples)
CC      := gcc
AS      := nasm
LD      := ld
QEMU    := qemu-system-i386

# Tenta recuperar o diretório de headers internos do GCC para tipos básicos (stdint.h, etc)
CC_INTERNAL_INC := $(shell $(CC) -m32 -print-file-name=include 2>/dev/null)

CFLAGS  := -m32 -std=c11 -ffreestanding -O2 -Wall -Wextra \
            -fno-stack-protector -fno-pie -fno-PIC -fno-stack-check \
            -mno-sse -mno-mmx -mno-sse2 -fcf-protection=none \
            -nostdinc -isystem "$(CC_INTERNAL_INC)" -Iinclude
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T scripts/linker.ld -z max-page-size=0x1000

C_SRCS  := kernel/kernel.c \
            kernel/arch/x86/gdt.c \
            kernel/arch/x86/idt.c \
            kernel/mm/pmm.c \
            kernel/mm/vmm.c \
            kernel/mm/heap.c \
            kernel/drivers/vga/vga.c \
            kernel/lib/string.c

ASM_SRCS := boot/multiboot.asm \
             kernel/arch/x86/gdt_flush.asm \
             kernel/arch/x86/idt_flush.asm

C_OBJS   := $(patsubst %.c,   build/%.o, $(C_SRCS))
ASM_OBJS := $(patsubst %.asm, build/%.o, $(ASM_SRCS))

KERNEL   := build/kernel.bin

.PHONY: all run clean

all: $(KERNEL)

$(KERNEL): $(ASM_OBJS) $(C_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

run: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -m 32M -display gtk

clean:
	rm -rf build
