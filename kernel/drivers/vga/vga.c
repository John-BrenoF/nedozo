/* ============================================================
   kernel/drivers/vga/vga.c — Driver de texto VGA (modo 80×25)
   O buffer de memória mapeada começa em 0xB8000.
   Cada caractere ocupa 2 bytes: [Cores(8)] [Caractere(8)]
   ============================================================ */
#include <drivers/vga.h>
#include <arch/io.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_ADDR   ((volatile uint16_t *)0xB8000)

static volatile uint16_t *vga_buf;
static int     cur_row, cur_col;
static uint8_t cur_color;

static inline uint16_t make_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga_buf[i] = make_entry(' ', cur_color);
    cur_row = cur_col = 0;
}

void vga_init(void) {
    vga_buf   = VGA_ADDR;
    cur_row   = 0;
    cur_col   = 0;
    cur_color = (VGA_BLACK << 4) | VGA_LIGHT_GREY;
    vga_clear();
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    cur_color = ((uint8_t)bg << 4) | (uint8_t)fg;
}

static void scroll_up(void) {
    for (int r = 1; r < VGA_HEIGHT; r++)
        for (int c = 0; c < VGA_WIDTH; c++)
            vga_buf[(r-1)*VGA_WIDTH + c] = vga_buf[r*VGA_WIDTH + c];
    for (int c = 0; c < VGA_WIDTH; c++)
        vga_buf[(VGA_HEIGHT-1)*VGA_WIDTH + c] = make_entry(' ', cur_color);
    cur_row = VGA_HEIGHT - 1;
}

/* Comunica com o hardware via portas de E/S para mover o cursor visual */
static void update_cursor(void) {
    uint16_t pos = cur_row * VGA_WIDTH + cur_col;
    outb(0x3D4, 0x0F); outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0x0E); outb(0x3D5, (pos >> 8) & 0xFF);
}

void vga_putchar(char c) {
    if (c == '\n') {
        cur_col = 0;
        if (++cur_row >= VGA_HEIGHT) scroll_up();
    } else if (c == '\r') {
        cur_col = 0;
    } else if (c == '\t') {
        cur_col = (cur_col + 8) & ~7;
        if (cur_col >= VGA_WIDTH) { cur_col = 0; if (++cur_row >= VGA_HEIGHT) scroll_up(); }
    } else {
        vga_buf[cur_row * VGA_WIDTH + cur_col] = make_entry(c, cur_color);
        if (++cur_col >= VGA_WIDTH) { cur_col = 0; if (++cur_row >= VGA_HEIGHT) scroll_up(); }
    }
    update_cursor();
}

void vga_puts(const char *s) {
    while (*s) vga_putchar(*s++);
}

/* Implementação de kprintf sem dependência de libc */
void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[32];
    for (; *fmt; fmt++) {
        if (*fmt != '%') { vga_putchar(*fmt); continue; }
        fmt++;
        switch (*fmt) {
            case 's': vga_puts(va_arg(ap, const char *)); break;
            case 'c': vga_putchar((char)va_arg(ap, int)); break;
            case 'd': {
                int32_t n = va_arg(ap, int32_t);
                int i = 0;
                if (n < 0) { vga_putchar('-'); n = -n; }
                if (n == 0) { buf[i++] = '0'; }
                else { while (n) { buf[i++] = '0' + n%10; n /= 10; } }
                while (i--) vga_putchar(buf[i]);
                break;
            }
            case 'u': {
                uint32_t n = va_arg(ap, uint32_t);
                int i = 0;
                if (n == 0) { buf[i++] = '0'; }
                else { while (n) { buf[i++] = '0' + n%10; n /= 10; } }
                while (i--) vga_putchar(buf[i]);
                break;
            }
            case 'x': {
                uint32_t n = va_arg(ap, uint32_t);
                const char *hex = "0123456789abcdef";
                int i = 0;
                if (n == 0) { buf[i++] = '0'; }
                else { while (n) { buf[i++] = hex[n&0xF]; n >>= 4; } }
                while (i--) vga_putchar(buf[i]);
                break;
            }
            case '%': vga_putchar('%'); break;
        }
    }
    va_end(ap);
}
