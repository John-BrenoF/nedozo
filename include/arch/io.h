/* ============================================================
   include/arch/io.h — Comunicação com Portas de Hardware (I/O)
   ============================================================ */
#ifndef ARCH_IO_H
#define ARCH_IO_H

#include <stdint.h>

/* Envia um byte para a porta especificada */
static inline void outb(uint16_t port, uint8_t value) {
    /* 'a' coloca o valor em AL, 'Nd' especifica a porta em DX ou constante immed */
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Lê um byte da porta especificada */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); /* '=a' lê de AL */
    return ret;
}

#endif /* ARCH_IO_H */
