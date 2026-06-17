/* ============================================================
   include/drivers/vga.h — Driver VGA Modo Texto
   ============================================================ */
#ifndef DRIVERS_VGA_H
#define DRIVERS_VGA_H

#include <stdint.h>

typedef enum {
    VGA_BLACK = 0, VGA_BLUE, VGA_GREEN, VGA_CYAN,
    VGA_RED, VGA_MAGENTA, VGA_BROWN, VGA_LIGHT_GREY,
    VGA_DARK_GREY, VGA_LIGHT_BLUE, VGA_LIGHT_GREEN,
    VGA_LIGHT_CYAN, VGA_LIGHT_RED, VGA_LIGHT_MAGENTA,
    VGA_LIGHT_BROWN, VGA_WHITE
} vga_color_t;

/* Inicializa posição, limpa tela e configura buffer em 0xB8000 */
void vga_init(void);

void vga_clear(void);               /* Preenche tela com espaços */
void vga_puts(const char *str);     /* Imprime string literal */
void vga_putchar(char c);           /* Imprime caractere e trata controle (\n, etc) */

/* Define cores de frente e fundo para as próximas escritas */
void vga_set_color(vga_color_t fg, vga_color_t bg);

/* printf simplificado para debug e logs do kernel */
void kprintf(const char *fmt, ...);

#endif /* DRIVERS_VGA_H */
