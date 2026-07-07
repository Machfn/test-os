#include "stdarg.h"
#include "tty.h"
#include "../libc/include/string.h"

#include <stdint.h> // Comment this out before build, purely for linting purpose

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint16_t* const VGA_MEMORY = (uint16_t *)0xb8000;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

static inline uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t make_vgaentry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    size_t y;
    for (y = 0; y < VGA_HEIGHT; ++y) {
        size_t x;
        for (x = 0; x < VGA_WIDTH; ++x) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = make_vgaentry(' ', terminal_color);
        }
    }
}

void terminal_scroll(void)
{
    for (int i = 0; i < VGA_HEIGHT; ++i) {
        memset(&terminal_buffer[i * VGA_WIDTH], make_vgaentry(' ', terminal_color) , VGA_WIDTH * sizeof(uint16_t));
        for (int j = 0; j < VGA_WIDTH; ++j) {
            terminal_buffer[i * VGA_WIDTH + j] = terminal_buffer[(i + 1) * VGA_WIDTH + j];
        }
        terminal_row--;
    }
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
    if (c == '\n' || c == '\r') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    } else if (c == '\t') {
        terminal_column += 4;
        return;
    } else if (c == '\b') {
        terminal_putentryat(' ', terminal_color, terminal_column--, terminal_row);
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

void terminal_write(const char *data, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        terminal_putchar(data[i]);
    }
}

int putchar(int ic)
{
    char c = (char)ic;
    terminal_write(&c, sizeof(c));
    return ic;
}

static void print(const char *data, size_t data_length)
{
    size_t i;
    for (i = 0; i < data_length; ++i) {
        putchar((int) ((const unsigned char *) data)[i]);
    }
}

static void print_uint(unsigned int value)
{
    char buf[19];
    size_t i = 0;

    if (value == 0) {
        putchar('0');
        return;
    }

    while (value > 0) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        putchar(buf[--i]);
    }
}

int printf(const char *format, ...)
{
    va_list parameters;
    va_start(parameters, format);
    int written = 0;
    size_t amount;
    int rejected_bad_specifier = 0;

    while (*format != '\0') {
        if (*format != '%') {
            print_c:
                amount = 1;
                while ( format[amount] && format[amount] != '%')
                    amount++;
                print(format, amount);
                format += amount;
                written += amount;
                continue;
        }

        const char *format_begun_at = format;

        if (*(++format) == '%') {
            goto print_c;
        }

        if (rejected_bad_specifier) {
            incomprehensible_conversion:
                rejected_bad_specifier = 1;
                format = format_begun_at;
                goto print_c;
        }

        if (*format == 'c') {
            format++;
            char c = (char)va_arg(parameters, int /* char promotes to int*/);
            print(&c, sizeof(c));
        } else if ( *format == 's') {
            format++;
            const char *s = va_arg(parameters, const char *);
            print(s, strlen(s));
        } else if ( *format == 'd') {
            format++;
            int d = va_arg(parameters, int);
            
            if (d < 0) {
                putchar('-');
                d = -d;
            }

            print_uint(d);
        } else {
            goto incomprehensible_conversion;
        }
    }
    va_end(parameters);

    return written;
}

int get_terminal_row(void)
{
    return terminal_row;
}

int get_terminal_col(void)
{
    return terminal_column;
}
